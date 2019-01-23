#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "new.h"
#include "String.h"


struct String {
    const void * class;
    char * text;
    struct String * next;
    unsigned count;
};

static struct String * ring;

static void * String_ctor(void * _self, va_list * args) {
    struct String * self = _self;
    const char * text = va_arg(* args, const char *);

    if (ring) {
        struct String * p = ring;
        do { // 找到已经存在的字符串，则返回已存在的
            if (strcmp(p -> text, text) == 0) {
                ++ p->count;
                free(self);
                return p;
            }
        } while ((p = p->next) != ring);
        /* ring 存在，但是没有找到已重复的
         * 将 self 插入到 ring 头后面*/
    } else {
        // 不存在 ring 则第一个自己形成环
        ring = self;
    }
    self -> next = ring -> next;
    ring -> next = self;
    // 新分配的内存，对象计数为 1 
    self -> count = 1;

    size_t length = strlen(text);
    self -> text = malloc(length + 1);
    assert(self -> text);
    strncpy(self -> text, text, length);

    return self;
}

static void * String_dtor(void * _self) {
    struct String * self = _self;

    if (--self->count >0) {
        return 0;
    }
    assert(ring);
    if (ring == self) {
        ring = self -> next;
    }
    if (ring == self) {
        ring = 0;
    } else {
        struct String * p = ring;
        while (p -> next != self) {
            p = p -> next;
            assert(p != ring);
        }
        p -> next  = self -> next;
    }

    free(self -> text), self -> text = 0;
    return self;
}

static void * String_clone(const void * _self) {
    struct String * self = (void *) _self;

    ++ self->count;
    return self;
}

static int String_differ(const void * _self, const void * _b) {
    const struct String * self = _self;
    const struct String * b = _b;

    if (self == b) {
        return 0;
    }

    if (!b || b -> class != String) {
        return 1;
    }

    return strcmp(self -> text, b -> text);
}


static const struct Class _String = {
    sizeof(struct String),
    String_ctor,
    String_dtor,
    String_clone,
    String_differ
};

const void * String = & _String;

