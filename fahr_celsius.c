#include <stdio.h>

#define LOWER 0
#define UPPER 300
#define STEP 20

int fahr_celsius() {
    double fahr, celsius;

    fahr = LOWER;
    while (fahr <= UPPER) {
        celsius = (5.0 / 9.0) * (fahr - 32.0);
        printf("%3.0f\t%6.1f\n", fahr, celsius);
        fahr = fahr + STEP;
    }
}

int fahr_for_celsius() {
    int fahr;

    for (fahr = UPPER; fahr >= LOWER; fahr -= STEP) {
        printf("%3d %6.1f\n", fahr, (5.0 / 9.0) * (fahr - 32));
    }
}
