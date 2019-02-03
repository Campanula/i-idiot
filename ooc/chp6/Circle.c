#include <stdio.h>

#include "Circle.h"
#include "Circle.r"

// 用于类型描述符的一系列定义
static void * Circle_ctor(void * _self, va_list * app) {
    // 调用父类构造器处理实例在父类中可以访问的部分 x, y
    struct Circle * self = super_ctor(Circle, _self, app);

    self -> rad = va_arg(* app, int);
    return self;
}

static void Circle_draw(const void * _self) {
    const struct Circle * self = _self;

    printf("circle at % d,%d rad %d\n", x(self), y(self), self -> rad);
}

const void * Circle;

void initCircle(void) {
    if (! Circle) {
        // 初始化需要依赖的 PointClass 和 Point
        initPoint();
        Circle = new(PointClass, "Circle",
                Point, sizeof(struct Circle),
                ctor, Circle_ctor,
                draw, Circle_draw,
                0);
    }
}

