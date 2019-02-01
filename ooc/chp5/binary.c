#include <string.h>


void * binary(
        const void * key,  // 指向 char * 的指针
        void * _base,      // symbol 列表 names 名字的拷贝, 指向第一个元素
        size_t * nelp,     // 指向 symbol 列表元素个数
        size_t width,      // 单个元素的大小
        int (* cmp) (const void * key, const void * elt)
    )
{
#define base    (* (char **) & _base)
    // _base 指向列表的第一个元素，这个列表的元素是 struct Name *
    // base 指向列表的第一个元素，这个列表的元素是 char *
    // 这里的本意是将其转换为以 char 为大小移动的指针

    size_t nel = * nelp;              // 元素个数，拷贝
    char * lim = base + nel * width;  // 最后一个元素之后

    if (nel > 0) {    // 如果元素个数大于零
        char * high;  //  二分查找闭区间 [base, high]
        for (high = lim - width; base <= high; nel >>= 1) {
            // 二分中点, base 展开为 _base，_base 跟踪闭区间的下限
            char * mid = base + (nel >> 1) * width;
            int c = cmp(key, mid);

            if (c < 0) {
                high = mid - width;
            } else if (c > 0) {
                // 由宏展开的变量，通过赋值，是否改变了_base 的值？
                // 改变了，但是没有改变调用方法search中的names，_base 和 names 都是一个指针，_base 是 names 的一个拷贝
                base = mid + width;
                -- nel;
            } else {
                return (void *) mid;
            }
        }
        // 将 base 指向位置的元素整体向后移动width距离，挪出空间给新的元素
        memmove(base + width, base, lim - base);
    }

    // 新增元素，symbol 列表元素个数加一
    ++ *nelp;
    // 将 key 指向的内容 (char *) 拷贝到base指向的位置
    // 占用大小为 width  (struct Name *)，其实就是一个指针大小
    // 返回指向这个位置的指针，在这里返回的是 char **
    return memcpy(base, key, width);

#undef base
}

