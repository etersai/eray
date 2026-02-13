#include "string.h"
#include "../lamath.h"
#include "types.h"
#include "arena.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct {
    u8* bytes;    
    u64 size;
} string8;

internal inline string8 str8(u8* str, u64 size)
{
    return (string8){str,size};
}

internal inline void str8_print(string8 str)
{
    fwrite(str.bytes, 1, str.size, stderr);
}

#define str8_lit(str) str8((u8*)(str), sizeof(str) - 1)

internal string8 str8_format(Arenka* arena, char* fmt, ...)
{
    unused(arena);
    u8 buf[KB(1)];
    va_list args;
    va_start(args, fmt);
    int num_chars_wo_null_byte = vsnprintf((char*)buf, sizeof(buf), fmt, args);
    va_end(args);
    u8* bytes = (u8*)malloc(num_chars_wo_null_byte);
    memcpy(bytes, buf, num_chars_wo_null_byte);
    return (string8){bytes, num_chars_wo_null_byte};
}

global Arenka scratch_arena;
int main(void)
{
    elog_s("PROGRAM INIT...");
    scratch_arena = arenka_map(MB(10000));
    if (scratch_arena.addr_start == NULL) {
        elog_abort("memory mapping failed");
    }
    arenka_unmap(&scratch_arena);
    return 0;
}
