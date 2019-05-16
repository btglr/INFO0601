#include "utils.h"
#include <math.h>

bool isPrime(int number) {
    int i;

    if (number <= 1)
        return false;

    if (number % 2 == 0 && number > 2)
        return false;

    for (i = 3; i < number / 2; i+= 2) {
        if (number % i == 0)
            return false;
    }

    return true;
}

int findDivisor(int number) {
    int i;

    /* Exclude 1 and 2 as we don't want chunks too small */
    for (i = 3; i <= number && number % i != 0; ++i) {}

    return i;
}
