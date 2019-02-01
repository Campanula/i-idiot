#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "Name.h"
#include "Name.r"
#include "parse.h"
#include "value.h"
#include "value.r"


// 拓展基类结构
// 只接收一个参数的函数，和一元操作符比较类似，我们可以拓展一元操作符，
// 但是为每一个函数都定义一个类似于 doMinus 的方法过于繁琐，这里使用二元的方式，
// 让左子树表示具体的函数。
// 不同于其他结构体，比如Bin，Var，Val，可以单独作为一个类型描述符对应的树节点
// 这个结构体只用来表示（函数，算术表达式）二元中的一元，只是 Bin 结构体的左子树
// 另外值得一提的是，Var 的 value 成员在被 Assign 赋值之前，也不能作为独立的节点
// 所以 Var 要首先作为 Assign 二元中的一元，只是 Bin 结构体的左子树
struct Math {
    struct Name _;
    double (* funct) (double);
};


// 展开指定树节点的 funct 成员
#define funct(tree) (((struct Math *) left(tree)) -> funct)


static double doMath(const void * tree) {
    double result = exec(right(tree));

    errno = 0;
    result = funct(tree)(result);
    if (errno) {
        error(
            "error in %s: %s",
            ((struct Math *) left(tree)) -> _.name,
            strerror(errno)
        );
    }
    return result;
}


static void freeMath(void * tree) {
    // 调用右节点的释放函数
    delete(right(tree));
    // 左节点是 Math 类型的节点，存在于 symbol names 中
    // 这里只释放 Bin 结构体中的左右指针，Math 节点在整个周期都存在
    free(tree);
}


static const struct Type _Math = {
    mkBin, doMath, freeMath
};
const void * Math = & _Math;


void initMath() {
    static const struct Math functions[] = {
        { { &_Math, "sin",   MATH },  sin },
        { { &_Math, "cos",   MATH },  cos },
        { { &_Math, "tan",   MATH },  tan },
        { { &_Math, "asin",  MATH },  asin },
        { { &_Math, "acos",  MATH },  acos },
        { { &_Math, "atan",  MATH },  atan },
        { { &_Math, "sinh",  MATH },  sinh },
        { { &_Math, "cosh",  MATH },  cosh },
        { { &_Math, "tanh",  MATH },  tanh },
        { { &_Math, "exp",   MATH },  exp },
        { { &_Math, "log",   MATH },  log },
        { { &_Math, "log10", MATH },  log10 },
        { { &_Math, "sqrt",  MATH },  sqrt },
        { { &_Math, "ceil",  MATH },  ceil },
        { { &_Math, "floor", MATH },  floor },
        { { &_Math, "abs",   MATH },  fabs },
        { { 0 } }
    };

    const struct Math * mp;
    for(mp = functions; mp -> _.name; ++ mp) {
        install(mp);
    }
}

