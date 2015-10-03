#include "memory.h"

void destroy(void *_data) {
    void **data = _data;
    free(*data);
    *data = NULL;
}

float *floatdup(float f) {

    float *mem = malloc(sizeof(float));

    if (!mem) {
        return NULL;
    }

    *mem = f;

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
