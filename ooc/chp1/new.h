#ifndef NEW_H
#define NEW_H

#include <stddef.h>

void * new(const void * type, ...);
void delete(void * item);

#endif

