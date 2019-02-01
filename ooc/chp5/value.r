#ifndef VALUE_R
#define VALUE_R

#include <stdarg.h>

// 作为类型描述符模板
struct Type {
    void * (* new) (va_list ap);
    double (* exec) (const void * tree);
    void (* delete) (void * tree);
};

double exec(const void * tree);


// 二元操作符
struct Bin {
    const void * type;  // 指向类型描述符
    void * left, * right;
};

#define left(tree) (((struct Bin *) tree) -> left)
#define right(tree) (((struct Bin *) tree) -> right)

// 二元操作符的构建接口，直接作为共有接口，提供给 Bin-Assign 和 Bin-Math
void * mkBin(va_list ap);
void freeBin(void * tree);

#endif

