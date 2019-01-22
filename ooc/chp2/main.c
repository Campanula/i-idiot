#include <stdio.h>

#include "new.h"
#include "Object.h"
#include "Set.h"

int main(int argc, char ** argv){
    // 集合 s
    void * s = new(Set);
    // 将一个对象放入 s，并返回这个对象
    void * a = add(s, new(Object));
    // 将一个对象放入 s，并返回这个对象
    void * b = add(s, new(Object));
    // c 对象不在集合中
    void * c = new(Object);

    // 集合 s 包含 a 和 b
    if (contains(s, a) && contains(s, b))
        puts("ok");

    // 集合 s 不包含 c
    if (contains(s, c))
        puts("contains?");

    // 将 a 重复放入 s 不会生成新的对象
    if (differ(a, add(s, a)))
        puts("differ?");

    // 将 a 从 s 中移开之后，s 将不再包含 a
    if (contains(s, drop(s, a)))
        puts("drop?");

    // 移除并释放对象 b
    delete(drop(s, b));
    // 移除一个不包含的对象，返回空，并正常处理空指针
    delete(drop(s, c));

    return 0;
}

