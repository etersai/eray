#ifndef DA_H_
#define DA_H_

#include "types.h"

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif /*DA_INIT_CAP*/

#define DA_TYPE(T) typedef struct {T* data; u64 count; u64 capacity;} Da##T;

#define da_size_in_bytes(da) ((da)->count*sizeof(*(da)->data))
#define da_free(da) do { free((da)->data); (da)->count = 0; (da)->capacity = 0; } while (0)
#define da_init(da, elem_num) do { \
    if ((da)->data == NULL) { \
        (da)->data = malloc((elem_num)*sizeof(*(da)->data)); \
        perma_assert((da)->data); \
        (da)->count = 0; \
        (da)->capacity = (elem_num); \
    } \
} while (0) 
#define da_append(da, val) do { \
     if ((da)->count >= (da)->capacity) { \
        if ((da)->data == NULL) { \
            (da)->data = malloc(DA_INIT_CAP*sizeof(*(da)->data)); \
            perma_assert((da)->data); \
            (da)->capacity = DA_INIT_CAP; \
        } \
        else \
        { \
            (da)->data = realloc((da)->data, da_size_in_bytes(da)*2); \
            perma_assert((da)->data); \
            (da)->capacity = (da)->capacity*2; \
        } \
    } \
    (da)->data[(da)->count++] = (val); \
} while (0) 

#endif /* DA_H_ */
