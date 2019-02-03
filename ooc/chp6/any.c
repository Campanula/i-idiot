#include "Object.h"


// 用于定义 Any 类型描述符的对象实例比较方法
static int Any_differ(const void * _self, const void * b) {
    return 0;
}

int main() {
    // 分配一个 struct Object 大小的内存，并且指向 Object 类型描述符
    void * o = new(Object);
    // 分配一个 struct Class 大小的内存，并且指向 替换了 name, size, super（还是指向Object类型描述符）, differ 的类型描述符
    const void * Any = new(
            Class, "Any",
            Object, sizeOf(o),  // 作为 Object 的子类，适用于 Object 的方法同样适用于 Any
            differ, Any_differ,
            0);

    // 并不用 struct 描述分配的大小，而是由上面类型描述符指定的大小直接分配内存，并且指向 Any 类型描述符
    void * a = new(Any);
    // Any 实例所占空间将被视作 struct Object 来解释，Any 描述符所指定的方法继承至 Object 描述符 

    // selector 调用打印方法
    puto(Any, stdout);
    puto(o, stdout);
    puto(a, stdout);

    // selector 调用比较方法
    if ( differ(o, o) == differ(a, a)) {
        puts("ok");
    }

    // 调用第一个实例的比较方法与第二个参数比较
    // 调用 o 的比较方法比较 a
    // 调用 a 的比较方法比较 o
    if (differ(o, a) != differ(a, o)) {
        puts("not commutative");
    }

    delete(o), delete(a);
    // 试图释放类型描述符，Any 是一个实例，这个实例的类型描述符是 Class
    // Class 类型描述符指定的析构方法不会返回真正的指针给free
    delete((void*) Any);

    return 0;
}

