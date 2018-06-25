#include <stdio.h>

int fahr_celsius() {
    double fahr, celsius;
    int lower, upper, step;

    lower = 0;
    upper = 300;
    step = 20;

    fahr = lower;
    while (fahr <= upper) {
        celsius = (5.0 / 9.0) * (fahr - 32.0);
        printf("%3.0f\t%6.1f\n", fahr, celsius);
        fahr = fahr + step;
    }
}

int fahr_for_celsius() {
    int fahr;

    for (fahr = 300; fahr >= 0; fahr -= 20) {
        printf("%3d %6.1f\n", fahr, (5.0 / 9.0) * (fahr - 32));
    }
}
