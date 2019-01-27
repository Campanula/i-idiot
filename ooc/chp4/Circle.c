#include <stdarg.h>
#include <stdio.h>

#include "new.h"
#include "Circle.h"


static void * Circle_ctor(void * _self, va_list * app) {
    /* 直接使用父类的描述符，调用父构造器 */
    struct Circle * self = ((const struct Class *) Point) -> ctor(_self, app);

    /* 初始化自己的部分 */
    self -> rad = va_arg(* app, int);
    return self;
}


/* 如果这里并不想覆盖父类实现自己的方法，要如何做 
 * 目前的权宜之计是，父类的方法并不使用 static 修饰
 * 在此申明一次原型，然后在类型描述符引用*/
static void Circle_draw(const void * _self) {
    const struct Circle * self = _self;

    /* 使用父类提供的访问方法，访问父类部分的 */
    printf("circle at %d,%d rad %d\n", x(self), y(self), self -> rad);
}

/* 如果子类想拥有自己的独特方法，独特方法需要在类型描述符里注册吗？
 * 要如何处理？ */

static const struct Class _Circle = {
    sizeof(struct Circle),
    Circle_ctor, 0,
    Circle_draw
};


const void * Circle = & _Circle;

