#include <stdio.h>

#include "fahr_celsius.h"
#include "echo_eof.h"

int main() {
    printf("hello, world\n");
    fahr_celsius();

    printf("\n\n----------\n\n");
    fahr_for_celsius();

    echo_eof();
}
