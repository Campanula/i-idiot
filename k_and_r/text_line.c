#include <stdio.h>

int get_line(char line[], int limit) {
    int c, i;

    for (i = 0; i <= limit && (c = getchar()) != EOF; ++i) {
        line[i] = c;
        if (c == '\n') {
            ++i;
            break;
        }
    }
    line[i] = '\0';
    return i;
}

void copy_line(char target[], char origin[]) {
    int i;
    
    i = 0;
    while ((target[i] = origin[i]) != '\0')
        ++i;
}

