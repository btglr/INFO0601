#include <stdio.h>
#include "utils.h"
#include "ncurses.h"

int* makeUnique(int *array, int *length) {
    int *unique;
    int i, j = 0, k = 0, data;

    unique = (int*) malloc(*length * sizeof(int));

    if (unique == NULL) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while trying to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < *length; ++i) {
        unique[i] = -1;
    }

    for (i = 0; i < *length; ++i) {
        data = array[i];

        for (j = 0; j < *length; ++j) {
            if (unique[j] == data) {
                break;
            }
        }

        if (j == *length) {
            unique[k] = data;
            k++;
        }
    }

    *length = k;

    return unique;
}