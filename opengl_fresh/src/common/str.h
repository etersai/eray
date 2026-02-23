#ifndef STR_H_
#define STR_H_

#include "types.h"
#include <stdbool.h>

typedef struct Arenka Arenka; 

typedef struct {
    u8* bytes;    
    u64 size;
} string8;

string8 str8(u8* str, u64 size);
string8 str8_cstr(const char* cstr);
bool str8_cmp_cstr(string8 str, const char* cstr);
bool str8_get_extension_after_dot(string8 str, string8* extension); /*"string_view" after first dot occurance*/
void str8_print(string8 str); // just for debug rn.
string8 str8_fmt(Arenka* arena, char* fmt, ...);

#define str8_lit(str) str8((u8*)(str), sizeof(str) - 1)
#define STR8_NULL (string8){NULL, 0}

#endif /* STR_H_ */
