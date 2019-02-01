#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Name.h"
#include "Name.r"
#include "parse.h"
#include "value.h"
#include "value.r"


// 拓展基类结构
struct Var {
    struct Name _;
    double value;
};


// 展开变量的值
#define value(tree) (((struct Var *) tree) -> value)

// 初始化结构体中的 struct Name 部分
static void nameInit(void * _self, const char * name, int token) {
    struct Name * self = (struct Name *) _self;
    size_t len = strlen(name);
    self -> name = malloc(len + 1);
    assert(self -> name);
    strncpy((void *) self -> name, name, len + 1);

    self -> token = token;
}


static void * mkVar(va_list ap) {
    /* 相对于预定义好的类型，比如：
     * { { &_Var, "PI",    CONST },  3.14159265358979323846 }
     * 运行时用户自定义的类型，其中的类型描述符由 new 处理
     * value 由 doAssign 处理，所以这里只需要处理 name 和 token
     * 这里也说明，在 symbol names 里的符号不一定都是初始化完全的*/
    struct Var * node = calloc(1, sizeof(struct Var));
    assert(node);

    // 分配好内存之后，只用初始化 name 和 token
    const char * name = va_arg(ap, const char *);
    nameInit(node, name, VAR);

    return node;
}


static double doVar(const void * tree) {
    // 直接返回 value，在 doVar 之前，一定有一个 doAssign 给 Var 节点的 value 成员赋值
    // doVar 是树的叶子节点，可以作为左值或者右值，作为左值既是 doAssign，作为右值既是 doVar
    return value(tree);
}


// 一系列 CONST symbol 在整个周期内都存在
static void freeVar(void * tree) {}


static double doAssign(const void * tree) {
    // 赋值行为，直接改变了在 symbol names 中用户自定义变量的值
    // 右子树是一个算术表达式树，而左子树是叶子节点
    return value(left(tree)) = exec(right(tree));
}


// Var 本身作为叶子节点，可以作为左值和右值，作为右值即是表示一个简单的值
static struct Type _Var = {mkVar, doVar, freeVar};
// 而作为左值，就是作为下面 Assign 二元操作符的左节点，表示被赋值
static struct Type _Assign = {mkBin, doAssign, freeBin};

const void * Var = & _Var;
const void * Assign = & _Assign;


void initConst() {
    static const struct Var constants[] = {
        { { &_Var, "PI",    CONST },  3.14159265358979323846 },
        { { &_Var, "E",     CONST },  2.71828182845904523536 },
        { { &_Var, "GAMMA", CONST },  0.57721566490153286060 },
        { { &_Var, "DEG",   CONST }, 57.29577951308232087680 },
        { { &_Var, "PHI",   CONST },  1.61803398874989484820 },
        { { 0 } }
    };

    const struct Var * vp;

    // 将上面符号的指针全部加入到 symbol names 中
    for (vp = constants; vp -> _.name; ++ vp) {
        install(vp);
    }
}


void * symbol;

int screen(const char * name) {
    // 在 symbol 列表中搜索指定的符号
    struct Name ** pp = search(& name);

    // 如果是新增的符号，说明是用户定义的
    if (* pp == (void *) name) {
        // 将在symbol names 中的值赋值为新的节点指针，并且是Var类型
        * pp = new(Var, name);
    }

    // 同时赋值给全局变量 symbol
    symbol = * pp;
    return (* pp) -> token;
}

