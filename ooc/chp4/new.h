#ifndef NEW_H
#define NEW_H

#include <stddef.h>
#include <stdarg.h>

struct Class {
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    void (* draw) (const void * self);
};

void * new(const void * type, ...);
void delete(void * item);

void draw(const void * self);

#endif

