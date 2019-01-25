#ifndef	VALUE_H
#define	VALUE_H


// 类型描述符，传递给new构建对象
const void * Add;
const void * Sub;
const void * Mult;
const void * Div;
const void * Minus;
const void * Value;

// 对象多态方法 selector
void * new(const void * type, ...);
void process(const void * tree);
void delete(void * tree);


#endif

