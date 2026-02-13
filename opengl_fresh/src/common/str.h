#ifndef STR_H_
#define STR_H_

#include "types.h"

typedef struct {
    u8 *str;    
    u64 size;
} string8;

#define str8_lit(S) str8((U8*)(S), sizeof(S) - 1)

#endif /* STR_H_ */
