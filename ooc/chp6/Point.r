#ifndef POINT_R
#define POINT_R

#include "Object.r"

// 拓展 struct Object，所有类都是 Object 类的子类
// 继承、并新增成员属性
struct Point {
    const struct Object _;
    int x, y;
};

// 获取实例的x成员值和y成员值
#define x(p) (((const struct Point *)(p)) -> x)
#define y(p) (((const struct Point *)(p)) -> y)

// 调用父类的绘制方法处理实例在父类中可以访问的部分
void super_draw(const void * class, const void * self);

// 拓展 struct Class，指定本类新的描述符将会多一个成员 draw
// 继承、覆写、并新增方法
struct PointClass {
    const struct Class _;
    void (* draw) (const void * self);
};
// 新增方法需要拓展 struct Class，以在类型描述符中可以指定新增方法

#endif

