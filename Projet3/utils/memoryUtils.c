#include <stdio.h>
#include "memoryUtils.h"
#include "../ncurses.h"

void *malloc_check(size_t length) {
    void *var;

    if ((var = malloc(length)) == NULL) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while trying to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    return var;
}
