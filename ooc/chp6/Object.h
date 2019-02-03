#ifndef OBJECT_H
#define OBJECT_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

extern const void * Object;

// 实例方法
void * new(const void * class, ...);
void delete(void * self);

// 实例工具
const void * classOf(const void * self);
size_t sizeOf(const void * self);

// selector
void * ctor(void * self, va_list * app);
void * dtor(void * self);
int differ(const void * self, const void * b);
int puto(const void * self, FILE * fp);

extern const void * Class;

// 类方法
const void * super(const void * self);

#endif

