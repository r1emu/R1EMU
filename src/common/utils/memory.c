#include "memory.h"


float *floatdup(float *f) {

    float *mem = malloc(sizeof(float));

    if (!mem) {
        return NULL;
    }

    *mem = *f;

    return mem;
}

int *intdup (int *i) {

    int *mem = malloc(sizeof(int));

    if (!mem) {
        return NULL;
    }

    *mem = *i;

    return mem;
}
