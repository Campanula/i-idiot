#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "new.h"

void * new(const void * _class, ...) {
    const struct Class * class = _class;
    void * p = calloc(1, class -> size);

    assert(p);

    // 将分配出的内存的开头位置赋值为指向类型描述符的指针
    *(const struct Class **) p = class;

    if (class -> ctor) {
        va_list ap;

        va_start(ap, _class);
        p = class -> ctor(p, & ap);
        va_end(ap);
    }
    return p;
}

void delete(void * self) {
    // 取出对象中头部的指向类型描述符的指针
    const struct Class ** cp = self;

    if (self && * cp && (* cp) -> dtor) {
        self = (* cp) -> dtor(self);
    }
    free(self);
}

void draw(const void * self) {
    const struct Class * const * cp = self;

    assert(self && * cp && (* cp) -> draw);
    (* cp) -> draw(self);
}

