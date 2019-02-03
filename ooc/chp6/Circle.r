#ifndef CIRCLE_R
#define CIRCLE_R

#include "Point.r"

// 拓展 struct Point，新增成员 rad
struct Circle {
    const struct Point _;
    int rad;
};

#endif

