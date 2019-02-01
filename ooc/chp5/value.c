#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


#include "parse.h"
#include "value.r"


/* selector 函数 */

void * new (const void * type, ...) {
    assert(type && ((struct Type *) type) -> new);

    va_list ap;
    void * result;

    va_start(ap, type);
    /* 调用类型描述符中各自的构建方法 */
    result = ((struct Type *) type) -> new(ap);
    /* 注意这个并没有上一章中所说的，本new方法并没有分配内存
     * 分配内存交给了类型描述符，这里将返回结果的开头指向类型描述符*/
    * (const struct Type **) result = type;
    va_end(ap);

    return result;
}


void delete (void * _tree) {
    struct Type ** tree = _tree ? (struct Type **) _tree : 0;

    assert(tree && * tree && (* tree) -> delete);
    (* tree) -> delete(tree);
}


double exec (const void * _tree) {
    struct Type ** tree = _tree ? (struct Type **) _tree : 0;

    assert(tree && * tree && (* tree) -> exec);
    return (* tree) -> exec(tree);
}


void process(const void * tree) {
    printf("\t%g\n", exec(tree));
}


/* 表示算数表达式中的数值token */
struct Val {
    /* 指向类型描述符，类型描述符在各方法定义之后定义 */
    const void * type;
    /* 浮点数值 */
    double value;
};

#define value(tree) (((struct Val *) tree) -> value)

static void * mkVal(va_list ap) {
    struct Val * node = malloc(sizeof(struct Val));

    assert(node);
    node -> value = va_arg(ap, double);
    return node;
}


static double doVal(const void * tree) {
    return value(tree);
}


/* 一元操作符 */
struct Un {
    const void * type;
    /* 一元操作符操作的对象是其他算数单元 */
    void * arg;  // 指向其他算数单元的指针
};

#define arg(tree) (((struct Un *) tree) -> arg)

static void * mkUn(va_list ap) {
    struct Un * node = malloc(sizeof(struct Un));

    assert(node);
    node -> arg = va_arg(ap, void *);
    return node;
}


static double doMinus(const void * tree) {
    return - exec(arg(tree));
}


static void freeUn(void * tree) {
    delete(arg(tree));
    free(tree);
}


/* 二元操作符 */

void * mkBin (va_list ap) {
    struct Bin * node = malloc(sizeof(struct Bin));

    assert(node);
    node -> left = va_arg(ap, void *);
    node -> right = va_arg(ap, void *);
    return node;
}


void freeBin(void * tree) {
    delete(left(tree));
    delete(right(tree));
    free(tree);
}


static double doAdd(const void * tree) {
    return exec(left(tree)) + exec(right(tree));
}


static double doSub(const void * tree) {
    return exec(left(tree)) - exec(right(tree));
}


static double doMult(const void * tree) {
    return exec(left(tree)) * exec(right(tree));
}


static double doDiv(const void * tree) {
    double left = exec(left(tree));
    double right = exec(right(tree));

    if (right == 0.0) {
        error("division by zero");
    }
    return left / right;
}


/* 各类型描述符 */
static struct Type _Add = {mkBin, doAdd, freeBin};
static struct Type _Sub = {mkBin, doSub, freeBin};
static struct Type _Mult = {mkBin, doMult, freeBin};
static struct Type _Div = {mkBin, doDiv, freeBin};
static struct Type _Minus = {mkUn, doMinus, freeUn};
/* Value 是 tree 的叶子节点，只需要释放自身即可 */
static struct Type _Value = {mkVal, doVal, free};

const void * Add = & _Add;
const void * Sub = & _Sub;
const void * Mult= & _Mult;
const void * Div = & _Div;
const void * Minus = & _Minus;
const void * Value = & _Value;

