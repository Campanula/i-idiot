#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "value.h"
#include "mathlib.h"
#include "Var.h"
#include "Name.h"


// 合法symbol组成字符
#define ALNUM \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
    "abcdefghijklmnopqrstuvwxyz" \
    "_0123456789"


// 解析出的算数表达式，由 scan 方法读取输入并赋值
static enum tokens token;
// 如果读取到的是数字，则赋值到 number
static double number;


// 每次读取一个token，赋值给全局变量 token
static enum tokens scan(const char * buf) {
    // 静态变量，指向正在处理的文本行
    static const char * bp;

    // 如果调用传参非空，说明是新的一行
    if (buf) {
        // 指向新的一行
        bp = buf;
    }
    // 另外，如果传参为空，则每一次调用，继续读取正在处理的行的下一个非空字符

    // 跳过所有的空格
    while (isspace(* bp)) {
        ++ bp;
    }

    // 如果读取到的字符是数字或者点开头，将其转换为浮点数
    if (isdigit(* bp) || * bp == '.') {
        // errno 定义在 errno.h，表示本线程范围内的错误码
        errno = 0;  // 每处理一个token，重新置为0

        token = NUMBER;
        // 将字符串转换为浮点数, 第一个参数是指向要处理的字符数组的指针
        // 第二个参数是指向终末符的指针，终末符本身是一个指针
        // 终末符将会被指向strtod处理后字符的下一个位置
        // 这里直接继续使用 bp 作为终末符，效果就是 bp 向前移动到后续待处理的字符的位置
        number = strtod(bp, (char **) & bp);
        // 检查错误码：范围错误
        if (errno == ERANGE) {
            // 进入错误处理，跳回 main()
            error("bad value: %s", strerror(errno));
        }
    // 如果读取到的字符是字母或者下划线开头，读出这一段名字
    } else if (isalpha(* bp) || * bp == '_') {
        // BUFSIZ 是定义在 stdio.h 的常量
        char buf [BUFSIZ];
        // 获取bp中由ALNUM字符组成的字符串的长度
        int len = strspn(bp, ALNUM);

        if (len >= BUFSIZ) {
            error("name too long: %-.10s...", bp);
        }

        // 拷贝出由字符和下划线组成的名字，并将 bp 后移
        // strncpy 并不保证目标数组以 null terminator 结尾
        strncpy(buf, bp, len), buf[len] = '\0', bp += len;
        // 查找符号列表，符号列表分为两类，一类是预先定义好的节点，另一类是用户定义的VAR
        // 这两类节点的 token 是 MATH/CONST/VAR/LET，节点会预先存入 symbol names
        // 其中用户定义的 VAR 在screen中生成，并处理 token 和 symbol
        // Var 可以作为左值和右值，作为右值前，其值必须在 Assign 中被作为左值处理
        token = screen(buf);
    } else {
        // 如果字符非空，获取值，并且 bp 指针后移
        // 否则不后移，并且 token 置为 0
        token =  * bp ? * bp ++ : 0;
    }
    // token 可能是运算符，也可能是数字
    return token;
}

static void * sum ();


/* factor: 因式因子
 * {+|-} NUMBER | CONST | VAR | (sum) | MATH(sum)
 * 读取算术表达式中的因式因子，因子可能是：
 * 1. 一个浮点数
 * 2. 括号高优先级的算数表达式
 * 3. 做正负变换的因子
 * 4. 常量或者变量
 * 5. 函数表达式*/
static void * factor () {
    void * result;

    switch ((int)token) {
        case '+':
            scan(0);
            return factor();
        case '-':
            scan(0);
            return new(Minus, factor());
        case NUMBER:  // 如果是数字，token 在 scan 方法中已经指向了下一个字符
            // 从全局变量number读取
            result = new(Value, number);
            break;
        case CONST:  // 常量只允许作为右值被当做值使用，不能作为左值被赋值
        case VAR:
            // 符号预先在 install 和 screen 方法中被构建到 symbol names 中
            result = symbol;  // 成为叶子节点
            break;
        case MATH:
        {   // 这里需要使用大括号，原因是：
            // 在 c 语法里，只允许存在 labeled statements 而没有 labeled declaration，如果第一句是一个申明，使用大括号将多个语句联合成一个 statement。
            // 在 c++ 语法里，是允许存在 labeled declaration 的，但是“初始化”是不能被case跳过的。深入的原因是，
            // switch 块是一个作用域，被申明的变量在接下来的域内是可见的，如果申明的变量自带初始化（显式赋值的申明或者隐式对象默认构造器），
            // 那么是不能被case跳过初始化的。这里使用大括号，将申明的变量约束在一个单独的case作用域里面，避免牵连到其他case跳过“初始化”。
            // 另外，
            // 针对c的解决办法还可以是使用 empty statement，也就是在case后面跟一个单独的没有语句的分号 ;。
            // 针对c++的解决办法还可以是，将申明和初始化分离（注意这对有默认构造器的对象是做不到的）成两句，一句申明，一句赋值（赋值不同于初始化），
            // 这样我们就只跳过了申明和赋值，而不是被认为跳过了“初始化”。
            // see https://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
            const struct Name * fp = symbol;
            if (scan(0) != '(') {
                error("expecting (");
            }
            scan(0);
            // 函数成为二元操作符的左节点，算术式或变量成为右节点
            result = new(Math, fp, sum());
            if (token != ')') {
                error("expecting )");
            }
            break;
        }
        case '(':
            scan(0);
            result = sum();
            if (token != ')') {
                error("expecting )");
            }
            break;
        default:
            error("bad factor: '%c' 0x%x", token, token);
    }
    scan(0);
    return result;
}


/* product: 乘积项，由因式因子和乘除符号组成
 * factor { *|/ factor } ... { *|/ factor } ...
 * 先读取一个因子，如果随后紧跟一个乘除符号，则读取另一个因子
 * 将乘除符号和两个因子组成新的因子，循环处理随后的乘除符号
 * 如果后面不再出现乘除符号，则返回当前的因子*/
static void * product () {
    void * result = factor();
    const void * type;

    while (1) {
        // 处理 token 是 *|/ 的情况，如果不是，直接返回结果
        switch ((int)token) {
            case '*':
                type = Mult;
                break;
            case '/':
                type = Div;
                break;
            default:
                return result;
        }
        scan(0);  // 当前 token 是 *|/，更新下一个待处理 token
        result = new(type, result, factor());
    }
}


/* sum: 算术表达式，由乘积项和加减符号组成
 * product { +|— product } ... { +|— product } ...
 * 先读取一个乘积项，如果随后紧跟一个加减符号，则读取另一个乘积项
 * 将加减号和两个项组成一个加减项，循环处理随后的加减符号
 * 如果后面不再出现加减符号，则返回当前的乘积项或者加减项*/
static void * sum () {
    void * result = product();
    const void * type;

    while (1) {
        // 处理 token 是 +|- 的情况，如果不是，直接返回结果
        switch ((int)token) {
            case '+':
                type = Add;
                break;
            case '-':
                type = Sub;
                break;
            default:
                // 可能是一个 Add|Sub 也可能是一个 Mult|Div
                return result;
        }
        scan(0);  // 当前 token 是 +|-，更新下一个待处理 token
        result = new(type, result, product());
    }
}

/* stmt: statement, 语句，由变量和算数表达式组成
 * {LET VAR =} sum
 * 可以包含赋值，也可以只包含算术表达式*/
static void * stmt() {
    void * result;

    switch((int)token) {
        case LET:
            if(scan(0) != VAR) {  // 只能是用户自定义的变量，不能是 CONST 或者 MATH
                error("bad assignment");
            }
            result = symbol;
            if(scan(0) != '=') {
                error("expecting =");
            }
            scan(0);
            // symbol 在 screen 中被构建到 symbol names 中
            // 这里作为二元操作符赋值的左节点，右节点是一个算术表达式
            return new(Assign, result, sum());
        default:
            return sum();
    }
}


// 由setjmp 使用，函数间跳转
static jmp_buf onError;


int main(int argc, char ** argv) {
    volatile int errors = 0;
    // BUFSIZ 是定义在 stdio.h 的常量
    static char buf [BUFSIZ];

    initName();    // 将预先定义的 LET 放入symbol names
    initConst();   // 将一类预先定义的 CONST 节点放入 symbol names
    initMath();    // 将一类预先定义的 Math 节点放入 symbol names
    // 用户定义的 symbol 将会在运行时扫描文本时生成节点放入 symbol names

    // 保存函数状态位置，可以由 longjmp 跳过来
    // 如果不是跳过来的，将会返回0
    if (setjmp(onError)) {
        ++ errors;
    }

    // gets 已经在C99被弃用，在C11移除
    // 使用 fgets 读取一行输入文本
    while (fgets(buf, sizeof buf, stdin)) {
        if (scan(buf)) {  // 将 token 指向第一个待处理的字符
            // 获取一个表达式语句
            void * e = stmt();
            if (token) {
                error("trans after sum");
            }
            // 调用这个表达式的处理方法
            process(e);
            // 调用这个表达式的资源回收方法
            delete(e);
        }
    }

    return errors > 0;
}


void error (const char * fmt, ...) {
    va_list ap;

    // 打印错误信息
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap), putc('\n', stderr);
    va_end(ap);

    // 跳到 main() 并让 setjmp 返回 1
    longjmp(onError, 1);
}

