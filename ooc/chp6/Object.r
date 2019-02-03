#ifndef OBJECT_R
#define OBJECT_R


// 所有的 object 开头都需要指向自己所属的类的描述符
struct Object {
    const struct Class * class;  // 在实例中，指向类型描述符
};

// Class 继承自 Object，并且拥有成员属性:
// 名字，父类，大小，构造、析构、比较、打印
struct Class {
    const struct Object _;  // 继承自 Object，在实例中，指向类型描述符
    const char * name;  // 类的名字

    // 在类型描述符中，指向父类的类型描述符
    // 对于 Class，类型描述符就是实例，所以对于 CLass，是在实例中指向父类的类型描述符
    // 但我们还是统一以 实例-> 类型描述符 -> 父类的类型描述符 的方式访问父类描述符
    const struct Class * super;
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    int (* differ) (const void * self, const void * b);
    int (* puto) (const void * self, FILE * fp);
};


// 使用实例的父类对应的方法处理实例在父类中可以访问的部分
void * super_ctor (const void * class, void * self, va_list * app);
void * super_dtor (const void * class, void * self);
int super_differ (const void * class, const void * self, const void * b);
int super_puto (const void * class, const void * self, FILE * fp);

#endif

