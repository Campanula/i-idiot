#ifndef CIRCLE_H
#define CIRCLE_H

#include "Point.h"

struct Circle {
    // 拓展父类
    // 如果需要多继承，这里如何处理？
    const struct Point _;
    int rad;
};

extern const void * Circle;

#endif

