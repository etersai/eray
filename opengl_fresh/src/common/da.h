#ifndef DA_H_
#define DA_H_

#include "types.h"

typedef struct {
    int* data;
    u64 count; 
    u64 capacity;
} intda;

#define da_append(da, val) do { \
    (da).                                 \
} while (0)


#endif /* DA_H_ */
