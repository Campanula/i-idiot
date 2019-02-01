#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "binary.h"
#include "parse.h"
#include "value.h"
#include "Name.r"


#ifndef NAMES
#define NAMES 4
#endif


static int cmp(const void * _key, const void * _elt) {
    const char * const * key = _key;
    const struct Name * const * elt = _elt;

    // 比较给定字符串是否和元素的name相等
    return strcmp(* key, (* elt) -> name);
}


struct Name ** search(const char ** name) {
    // symbol 列表在这个函数中被初始化并分配内存
    // 外部变量和静态变量默认被初始化为零
    static const struct Name ** names;
    static size_t used, max;

    // 如果 symbol 列表 names 已经使用空间的数量达到最大，那么倍增 names 的内存空间大小
    if (used >= max) {
        names = names
            // 如果 names 已经分配过内存，那么倍增它
            ? realloc(names, (max *= 2) * sizeof * names)
            // 如果 names 还没有被分配过内存，以 NAMES 个 struct Name 指针的大小初始化它
            : malloc((max = NAMES) * sizeof * names);
        assert(names);
    }

    // 要搜索或者插入的name，symbol 有序列表（起点），已存在元素个数，单个元素大小，比较函数
    // 因为上面可能倍增 names 指向的空间，所以需要已存在元素个数作为边界
    return binary(name, names, & used, sizeof * names, cmp);
    // 返回已存在，或者新插入的位置的指针
}


void install(const void * np) {
    const char * name = ((struct Name *) np) -> name;
    // & name 是 char **，search 会取 char * 拷贝到 symbol names 合适的位置
    // 如果不重复，search 会返回指向这个新位置的指针，一个 char **
    // 如果重复，search 会返回一个指针，指向之前就已经存在的 char *，也是一个 char **
    struct Name ** pp = search(& name);

    // 在 char * 层次，比较我们期望插入的 char * 和 search 返回的 char * 是否相等。
    if (* pp != (void *) name) {
        error("cannot install name twice: %s", name);
    }
    // 把我们刚刚插入到 symbol names 中的表示 name 的 char * 替换为表示 np 的 struct Name *
    // 反正大家都是一个指针是不是？就这么从成员提升为整个struct了
    // 套一层指针直接写内存
    * pp = (struct Name *) np;
}


// 基类，可以没有描述符，也意味着没有引用描述符的实例

void initName() {
    static const struct Name names[] = {
        {0, "let", LET},
        {0},
    };

    const struct Name * np;

    for (np = names; np -> name; ++ np) {
        install(np);
    };
}

