#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Object.h"
#include "Object.r"


/* 用于 Object 类型描述符的一系列定义 */

// 通过 ctor selector 最终会调用到类描述符中类自己指定的构造函数
static void * Object_ctor(void * self, va_list * app) {
    // struct Object 只有一个指向描述符的指针，而这个指针由 new 方法负责处理
    // 所以这里并不需要额外的工作
    return self;
}

// 通过 dtor selector
static void * Object_dtor(void * self) {
    // struct Object 没有动态分配内存的成员，这里也不需要额外的处理
    return self;
}

// 通过 differ selector
static int Object_differ(const void * _self, const void * b) {
    // 判断两个指针是否相等
    return _self != b;
}

// 通过 puto selector
static int Object_puto(const void * _self, FILE * fp) {
    // 通过实例取得类型描述符
    const struct Class * class = classOf(_self);

    // 根据类型描述符打印类型信息
    return fprintf(fp, "%s at %p\n", class -> name, _self);
}


/* 通用工具 */

// 获取实例的类型描述符
const void * classOf(const void * _self) {
    // 任何实例都是继承自Object，也就是头部一定指向类型描述符
    const struct Object * self = _self;

    assert(self && self -> class);
    // 返回 class 成员，也是实例开头位置的一个指针
    // 指向的是实例所属类的类型描述符
    return self -> class;
}

// 通过实例的类型描述符获取实例的大小
size_t sizeOf(const void * _self) {
    // 通过实例拿到类型描述符
    const struct Class * class = classOf(_self);
    // 直接返回类型描述符指定的大小，因为当初在 new 里面是按照这个大小分配内存的
    // 如果成员是动态分配的内存，大小不算在内，只计为一个指针的大小
    return class -> size;
}


/* 用于 Class 类型描述符的一系列定义 */

// 通过 ctor selector 调用，而其他参数最初是由 new 传递过来的
static void * Class_ctor(void * _self, va_list * app) {
    // struct Class 比 struct Object 复杂，有更多的成员需要初始化

    // 初始化名字、父类、大小（初始化出的对象本身是一个描述符）
    struct Class * self = _self;
    self -> name = va_arg(* app, char *);
    self -> super = va_arg(* app, struct Class *);
    self -> size = va_arg(* app, size_t);
    assert(self -> super);

    // 从父类的描述符开始，将成员函数全部拷贝到本次初始化的实例中（实例是一个类型描述符）
    // 这样的好处是，修改父类方法和顺序，并不需要处理子类，子类只需要管理自己的部分
    // 这里为什么不直接从头开始拷贝？因为头部被new处理为本实例所属的文件描述符了
    // 那new可以将这个处理放到调用构造函数之后吗？至少new通过ctor selector调用本方法的时候，本实例的开头就要是文件描述符了
    const size_t offset = offsetof(struct Class, ctor);
    memcpy(
        (char *) self + offset,  // 目标起点，本实例
        (char *) self -> super + offset,  // 源起点，父类类型描述符
        sizeOf(self -> super) - offset  // 从 ctor 成员开始到结尾的字节大小，注意是以父类为基准，子类可能拓展更多空间和属性
    );

    {
        // 定义一个类型 voidf，这个类型表示一个不接收参数并且不返回内容的函数的指针
        typedef void (* voidf) ();
        voidf selector;  // selector 是一个不接收参数不返回内容的函数指针

        // 将可变参数列表复制一份
        // 构造器是从父类开始处理，直到new直接调用的子类构造器
        // 这个顺序是固定的，也就是说，每个构造器能够处理的特定函数是有限的
        // 但是我们希望传递给 new 的函数列表是任意顺序的
        // 所以这里进行一次拷贝，把包含有所有函数列表的可变参数列表复制一份
        // 每个子类和父类构造器都会遍历一遍全部的可能的函数列表
        va_list ap; va_copy(ap, * app);

        // 依次读取 selector 函数指针类型 和 本实例（一个类型描述符）要覆写父类对应的函数的函数指针
        while ((selector = va_arg(ap, voidf))) {
            voidf method = va_arg(ap, voidf);

            // 按照对应关系逐一覆盖，注意对构造和析构的调用被包装在了new和delete里
            // 但是这里依然需要 ctor 和 dtor 两个 selector 用于 selector/method 对
            if (selector == (voidf) ctor) {
                * (voidf *) & self -> ctor = method;
            } else if (selector == (voidf) dtor) {
                * (voidf *) & self -> dtor = method;
            } else if (selector == (voidf) differ) {
                * (voidf *) & self -> differ = method;
            } else if (selector == (voidf) puto) {
                * (voidf *) & self -> puto = method;
            }
        }
        va_end(ap);

        return self;
    }
}

static void * Class_dtor(void * _self) {
    struct Class * self = _self;

    // 本实例是一个类型描述符，不应该被释放
    fprintf(stderr, "%s: cannot destriy class\n", self -> name);
    return 0;
}


// 接收一个类型描述符指针，返回它父类的类型描述符指针
const void * super(const void * _self) {
    const struct Class * self = _self;

    assert(self && self -> super);
    return self -> super;
}


// 所有的类型描述符都是 Object 类的子类，包括 Class 和 Object 自己 （super 成员（父类）指向 Object）
// 所有的类型描述符都是 Class 类的实例，包括 Object 和 Class 自己 （class 成员 （类型描述符）指向 Class）
static const struct Class object [] = {
    {  // Object 类型描述符，是一个 Class 的实例，这个实例的头部指向 Class 类型描述符
        {object + 1},  // 指向这里的第二个元素
        "Object", object,  // Object 作为一个类型描述符，是自己的父类
        sizeof(struct Object),  // 一个 Object 类的实例的大小
        Object_ctor, Object_dtor,
        Object_differ, Object_puto
    }, {  // Class 类型描述符，是一个 Class 的实例，这个实例的头部指向 Class 类型描述符，也即是自己
        {object + 1},  // 指向第二个元素，也即是自己
        "Class", object,  // Class 作为一个类型描述符，它继承自 Object 类
        sizeof(struct Class),  // 一个 Class 类的实例的大小
        Class_ctor, Class_dtor,  // 拥有自己的构造和析构函数，用于重置实例内存值
        Object_differ, Object_puto  // 继承 Object 类的比较和打印方法
    }
};


const void * Object = object;
const void * Class = object + 1;


void * new(const void * _class, ...) {
    // 类型描述符都是 Class 的实例
    const struct Class * class = _class;
    assert(class && class -> size);

    // 根据类型描述符中指定的大小分配内存, 所有的实例都有与 struct Object 类似的布局
    struct Object * object = calloc(1, class -> size);
    assert(object);

    // 将实例内存的起始位置指向类型描述符
    // 前面章节是使用的 struct Class ** p 然后使用 * p = class
    // 这里怎么老老实实的使用成员属性 object -> class 了？
    // 因为在规定 Object 之前，对象的成员可能叫做 class, 也可能叫做type，
    // 现在可以统一为 object -> class 了。
    // 其实我一开始就有这个想法，一开始就约定好多方便。
    object -> class = class;

    va_list ap;
    va_start(ap, _class);

    // Object 是 Class 的实例，Class 约定一个类都应该提供构造方法
    // 常规调用 selector 方法执行构造函数，初始化内存值
    // new 分配内存后，使用了 selector 方式在这一块内存上调用构造函数
    // 所以在真正调用构造函数之前，这块内存的头部就要指向类型描述符
    // 那么这里可以不使用 selector 的方式调用构造函数吗？
    // selector 可以负责检查类型描述符中是否一定指定了ctor成员
    object = ctor(object, & ap);
    // 这样调用似乎并没有问题，除非类型描述符不提供构造函数
    // 书中说之后可能决定动态绑定 new 和 delete 方法，目前还不知道是什么意思
    /* object = class -> ctor(object, & ap); */

    va_end(ap);
    return object;
}


void delete(void * _self) {
    if (_self) {
        // selector 方法调用析构函数，然后释放析构函数返回指针指向的内存
        free(dtor(_self));
    }
}


void * ctor(void * _self, va_list * app) {
    // 取出实例的类型描述符（实例由new方法分配内存，并指定类型描述符）
    const struct Class * class = classOf(_self);

    assert(class -> ctor);
    // 调用类型描述符中指定的构造函数
    return class -> ctor(_self, app);
}

// 这里接口为什么要提供显式的 class？这个 class 不能从self这个实例的头部取吗？
// 不能，因为实际上子类中调用父类方法，每一层向上的时候，都是同一块内存空间（同一个实例，只是属性的可见性不同）
// 但是每一个父类一次向上的时候，class 并不相同，并不是实例中的那个文件描述符
// 总结：同一块内存空间，被每一个父类的函数依次处理
void * super_ctor(const void * _class, void * _self, va_list * app) {
    // 接收一个类型描述符，一个实例
    // 取出类型描述符指定的父类类型描述符
    const struct Class * superclass = super(_class);

    assert(_self && superclass -> ctor);
    // 由父类的类型描述符指定的构造函数初始化实例在父类可以访问的部分
    return superclass -> ctor(_self, app);
}


void * dtor(void * _self) {
    // 取出实例的类型描述符
    const struct Class * class = classOf(_self);

    assert(class -> dtor);
    // 调用类型描述符指定的析构函数
    return class -> dtor(_self);
}

void * super_dtor(const void * _class, void * _self) {
    // 接收一个类型描述符和一个实例
    // 取出类型描述符指定的父类类型描述符
    const struct Class * superclass = super(_class);

    assert(_self && superclass -> dtor);
    // 调用父类描述符指定的析构函数析构实例在父类中可以访问的部分
    return superclass -> dtor(_self);
}

int differ(const void * _self, const void * b) {
    // 取出实例的类型描述符
    const struct Class * class = classOf(_self);

    assert(class -> differ);
    // 根据描述符调用比较方法
    return class -> differ(_self, b);
}

int super_differ(const void * _class, const void * _self, const void * b) {
    const struct Class * superclass = super(_class);

    assert(_self && superclass -> differ);
    // 调用父类描述符指定的比较方法
    return superclass -> differ(_self, b);
}

int puto(const void * _self, FILE * fp) {
    const struct Class * class = classOf(_self);

    assert(class -> puto);
    // 根据类型描述符指定的打印方法打印实例和类信息
    return class -> puto(_self, fp);
}

int super_puto(const void * _class, const void * _self, FILE * fp) {
    const struct Class * superclass = super(_class);

    assert(_self && superclass -> puto);
    // 调用父类描述符指定的方法打印实例和类信息
    return superclass -> puto(_self, fp);
}

