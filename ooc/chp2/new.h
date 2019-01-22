#ifndef NEW_H
#define NEW_H

#include <stddef.h>
#include <stdarg.h>

struct Class {
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    void * (* clone) (const void * self);
    int (* differ) (const void * self, const void * b);
};

void * new(const void * type, ...);
void delete(void * item);

void * clone(const void * self);
int differ(const void * self, const void * b);

size_t sizeOf(const void * self);

#endif

