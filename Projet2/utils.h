#ifndef __PROJET2_UTILS_H__
#define __PROJET2_UTILS_H__

#include <stdlib.h>

#undef _GNU_SOURCE
#define _GNU_SOURCE

/**
 * Takes an array of integers and returns a new array with unique values
 * @param arr The input array
 * @param length The length of the array
 * @return A new array containing the values
 */
int* makeUnique(int *arr, int* length);

#endif