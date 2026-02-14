#ifndef STR_H_
#define STR_H_

#include "types.h"

typedef struct Arenka Arenka; 

typedef struct {
    u8* bytes;    
    u64 size;
} string8;

string8 str8(u8* str, u64 size);
void str8_print(string8 str);
string8 str8_fmt(Arenka* arena, char* fmt, ...);

#define str8_lit(str) str8((u8*)(str), sizeof(str) - 1)
#define STR8_NULL (string8){NULL, 0};

#endif /* STR_H_ */
