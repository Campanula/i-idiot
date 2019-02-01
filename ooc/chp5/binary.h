#ifndef BINARY_H
#define BINARY_H

#include <stddef.h>

// 一个维护有序列表的工具方法

void * binary(const void * key, void * base, size_t * nelp,
        size_t width, int (* cmp) (const void * key, const void * elt));

#endif

