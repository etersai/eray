#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "elog.h"

typedef struct {
    float* data;
    size_t count; 
    size_t capacity;
} intda;

void log_da(intda da)
{
    elog_p(da.data);
    elog_zu(da.count);
    elog_zu(da.capacity);
}

#define DA_INIT_CAP 1024

#define da_append(da, val) do { \
     if ((da)->count >= (da)->capacity) { \
        if ((da)->data == NULL) { \
            (da)->data = malloc(DA_INIT_CAP*sizeof(*(da)->data)); \
            (da)->capacity = DA_INIT_CAP; \
        } \
        else \
        { \
            (da)->data = realloc((da)->data, (da)->capacity*2*sizeof(*(da)->data)); \
            (da)->capacity = (da)->capacity*2; \
        } \
    } \
    (da)->data[(da)->count++] = (val); \
} while (0) 

#define da_free(da) do { free((da)->data); (da)->count = 0; (da)->capacity = 0; } while (0)

int main(void) {
    intda da = {0};
    log_da(da);
    elog_s("------");
    for (int i = 0; i < 25348; i++) {
        da_append(&da, (float)i); 
    }
    
    for (int i = 0; i < da.count; i++) {
        elog_f(da.data[i]);
    }

    log_da(da);
    elog_zu(da.capacity*sizeof(float));

    da_free(&da);
    return 0;
}
