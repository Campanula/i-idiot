#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


#include "parse.h"
#include "value.h"


/* 将 exec 改为不返回值，并且按照逆波兰式打印信息 */

/* 类型描述符结构体*/
/* 先定义各类型一致公认的委托结构
 * 然后各类型在这个委托结构上实现各自的方法*/
struct Type {
    /* 注意这里并没有上一章中所说的size */ 
    const char * name;
    void * (* new) (va_list ap);
    void (* exec) (const void * tree);
    void (* delete) (void * tree);
};


/* selector 函数 */

void * new (const void * type, ...) {
    assert(type && ((struct Type *) type) -> new);

    va_list ap;
    void * result;

    va_start(ap, type);
    /* 调用类型描述符中各自的构建方法 */
    result = ((struct Type *) type) -> new(ap);
    /* 注意这个并没有上一章中所说的，本new方法并没有分配内存*/
    * (const struct Type **) result = type;
    va_end(ap);

    return result;
}


void delete (void * _tree) {
    struct Type ** tree = _tree ? (struct Type **) _tree : 0;

    assert(tree && * tree && (* tree) -> delete);
    (* tree) -> delete(tree);
}


static void exec (const void * _tree) {
    struct Type ** tree = _tree ? (struct Type **) _tree : 0;

    assert(tree && * tree && (* tree) -> exec);
    (* tree) -> exec(tree);
}


void process(const void * tree) {
    putchar('\t');
    exec(tree);
    putchar('\n');
}


/* 表示算数表达式中的数值token */
struct Val {
    /* 指向类型描述符，类型描述符在各方法定义之后定义 */
    const void * type;
    /* 浮点数值 */
    double value;
};


static void * mkVal(va_list ap) {
    struct Val * node = malloc(sizeof(struct Val));

    assert(node);
    node -> value = va_arg(ap, double);
    return node;
}


static void doVal(const void * tree) {
    printf(" %g", ((struct Val *) tree) -> value);
}


/* 一元操作符 */
struct Un {
    const void * type;
    /* 一元操作符操作的对象是其他算数单元 */
    void * arg;  // 指向其他算数单元的指针
};


static void * mkUn(va_list ap) {
    struct Un * node = malloc(sizeof(struct Un));

    assert(node);
    node -> arg = va_arg(ap, void *);
    return node;
}


static void doUn(const void * tree) {
    exec(((struct Un *) tree) -> arg);
    printf(" %s", (* (struct Type **) tree) -> name);
}


static void freeUn(void * tree) {
    delete(((struct Un *) tree) -> arg);
    free(tree);
}


/* 二元操作符 */
struct Bin {
    const void * type;
    void * left, * right;
};


static void * mkBin (va_list ap) {
    struct Bin * node = malloc(sizeof(struct Bin));

    assert(node);
    node -> left = va_arg(ap, void *);
    node -> right = va_arg(ap, void *);
    return node;
}


static void freeBin(void * tree) {
    delete(((struct Bin *) tree) -> left);
    delete(((struct Bin *) tree) -> right);

    free(tree);
}


static void doBin(const void * _tree) {
    struct Bin * tree = (struct Bin *) _tree;
    exec(tree -> left);
    exec(tree -> right);
    printf(" %s", (* (struct Type **) tree) -> name);
}


/* 各类型描述符 */
static struct Type _Add = {"+", mkBin, doBin, freeBin};
static struct Type _Sub = {"-", mkBin, doBin, freeBin};
static struct Type _Mult = {"*", mkBin, doBin, freeBin};
static struct Type _Div = {"/", mkBin, doBin, freeBin};
static struct Type _Minus = {"_", mkUn, doUn, freeUn};
/* Value 是 tree 的叶子节点，只需要释放自身即可 */
static struct Type _Value = {"", mkVal, doVal, free};

const void * Add = & _Add;
const void * Sub = & _Sub;
const void * Mult= & _Mult;
const void * Div = & _Div;
const void * Minus = & _Minus;
const void * Value = & _Value;

