#ifndef VALUE_H
#define VALUE_H

const void * Minus;
const void * Value;
const void * Mult;
const void * Div;
const void * Add;
const void * Sub;

void * new (const void * type, ...);
void process(const void * tree);
void delete(void * tree);

#endif

