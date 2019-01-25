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
    // 另外，如果传参为空，则继续读取正在处理的行的下一个非空字符

    // 跳过所有的空格
    while (isspace(* bp)) {
        ++ bp;
    }

    // 如果读取到的字符是数字或者点开头，将其转换为浮点数
    if (isdigit(* bp) || * bp == '.') {
        // errno 定义在 errno.h，表示本线程范围内的错误码
        errno = 0;  // 每处理一个token，重新置为0

        token = TK_NUM;
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
 * {+|-} TK_NUM | (sum)
 * 读取算术表达式中的因式因子，因子可能是：
 * 1. 一个浮点数
 * 2. 括号高优先级的算数表达式
 * 3. 做正负变换的因子*/
static void * factor () {
    void * result;

    switch (token) {
        case '+':
            scan(0);
            return factor();
        case '-':
            scan(0);
            return new(Minus, factor());
        case '(':
            scan(0);
            result = sum();
            if (token != ')') {
                error("expecting )");
            }
            break;
        case TK_NUM:
            result = new(Value, number);
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
        switch (token) {
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
        switch (token) {
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


// 由setjmp 使用，函数间跳转
static jmp_buf onError;


int main(int argc, char ** argv) {
    volatile int errors = 0;
    // BUFSIZ 是定义在 stdio.h 的常量
    char buf [BUFSIZ];

    // 保存函数状态位置，可以由 longjmp 跳过来
    // 如果不是跳过来的，将会返回0
    if (setjmp(onError)) {
        ++ errors;
    }

    // gets 已经在C99被弃用，在C11移除
    // 使用 fgets 读取一行输入文本
    while (fgets(buf, sizeof buf, stdin)) {
        if (scan(buf)) {  // 将 token 指向第一个待处理的字符
            // 获取一个表达式
            void * e = sum();
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

