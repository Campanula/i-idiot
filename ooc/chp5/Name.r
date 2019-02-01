#ifndef NAME_R
#define NAME_R

// 基类结构体，用于子类拓展
struct Name {
    const void * type;
    const char * name;
    int token;
};

#endif

