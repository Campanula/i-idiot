#include <assert.h>

#include "Point.h"
#include "Point.r"

// 用于定义 Point 类型描述符的一系列方法
static void * Point_ctor(void * _self, va_list * app) {
    // 调用 Point 描述符的父类描述符指定的构造函数
    // 处理实例在父类中可以访问的部分
    struct Point * self = super_ctor(Point, _self, app);

    // 处理本实例中独有的成员属性
    self -> x = va_arg(* app, int);
    self -> y = va_arg(* app, int);

    return self;
}

// 拓展类型描述符后指定的新增方法，通过 selector 会被调用
static void Point_draw(const void * _self) {
    const struct Point * self = _self;

    printf("\".\" at %d,%d\n", self -> x, self -> y);
}


// selector 访问实例的绘制方法
void draw(const void * _self) {
    // 取出实例的类型描述符
    const struct PointClass * class = classOf(_self);

    assert(class -> draw);
    // 调用类型描述符指定的绘制方法
    class -> draw(_self);
}

void super_draw(const void * _class, const void * _self) {
    // 从类型描述符取出父类类型描述符
    const struct PointClass * superclass = super(_class);

    assert(_self && superclass -> draw);
    // 使用父类描述符指定的绘制方法处理实例在父类中可以访问的部分
    superclass -> draw(_self);
}

// move 并不是一个 selector，而是直接调用，这将可以强制被继承并适用于子类
void move(void * _self, int dx, int dy) {
    struct Point * self = _self;
    self -> x += dx, self -> y += dy;
}

// 有 super_draw 这里却没有super_move
// 绘制可以在原有方法上拓展，但是 move 不可以

static void * PointClass_ctor(void * _self, va_list * app) {
    // 首先调用父类构造器处理好本实例（类型描述符）的常规成员部分
    // name, super, size, ctor, dtor, differ, puto 部分
    struct PointClass * self = super_ctor(PointClass, _self, app);

    typedef void (* voidf) ();
    voidf selector;  // 函数指针

    // 为什么这里总是要复制一份可变参数列表？
    // 实际上上面调用的父类构造器只会消费可变参数列表中的基本参数，比如 name, super, size
    // 而所有的 selector/method 都通过拷贝来消费，原副本中的 selector/method 并没有被消费
    // 这里依照惯例依然使用拷贝来处理 selector/method 的消费，以照顾可能存在的继承该描述符的子类
    // 这里这样做的原因是，具体来说，selector/method 在参数列表中出现的顺序不是固定的
    // 可能先是子类需要的，也可能先是父类需要的，但调用父类构造器的顺序却是固定的。
    // 如果父类不使用拷贝，把唯一一份参数列表给消费了，那么子类所需要的 selector/method 可能就被父类弄丢了
    va_list ap; va_copy(ap, * app);
    // 当然，这样设计，父类依然可以掌握主动权，选择性的真实消费参数列表，让子类没办法处理新的方法

    // 依次取出对应的 selector 和指定的覆写的方法
    while ((selector = va_arg(ap, voidf))) {
        voidf method = va_arg(ap, voidf);

        // 处理新增、覆写的 draw 方法
        if (selector == (voidf) draw) {
            * (voidf *) & self -> draw = method;
        }
    }

    va_end(ap);  // 立刻结束副本

    return self;
}

const void * PointClass, * Point;

void initPoint(void) {
    if (! PointClass) {  // 如何构造一个类
        // 新的类型描述符模板，允许构建出的类型描述符多出一种 draw 方法
        // 以 struct Class 大小分配内存，指向 Class 类型描述符，是 Class 的实例
        // super 指向 Class 类型描述符，是 Class 的子类
        // 只覆写了ctor方法，没有增加方法
        PointClass = new(Class, "PointClass",
                // 这里的sizeof大小是 new 实例的大小，还是new出描述符中成员size的大小？
                // 最终处理这个参数的是 Class_ctor，这里的 sizeof 最终会放入 PointClass 这个类型描述符的 size 成员
                Class, sizeof(struct PointClass),
                ctor, PointClass_ctor,  // 类型描述符的构造方法，在方法中指明了这个描述符会大一点，多包含一个方法
                0);
    }
    if (! Point) {  // 在如何构造一个类之后，根据这个构造出的类，如何构造一个点
        // 以 struct PointClass 大小分配内存，指向 PointClass 类型描述符，是 PointClass 的实例，实例本身依然是一个类型描述符
        // super 指向 Object 描述符，是 Object 的子类
        Point = new(PointClass, "Point",
                Object, sizeof(struct Point),
                ctor, Point_ctor,  // 点实例的构造方法
                draw, Point_draw,
                0);
    }
}

