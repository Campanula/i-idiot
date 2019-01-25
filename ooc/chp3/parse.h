#ifndef PARSE_H
#define PARSE_H

enum tokens {
    TK_NUM = 'n',
    TK_ADD = '+',
    TK_SUB = '-',
    TK_MUL = '*',
    TK_DIV = '/',
    TK_NEG = '-',
    TK_BKT = '(',
};

void error(const char * fmt, ...);

#endif

