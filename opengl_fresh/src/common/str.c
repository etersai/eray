#include "str.h"
#include "string.h" // memcpy
#include "types.h"
#include "arena.h"
#include <stdio.h>
#include <stdarg.h>

string8 str8(u8* str, u64 size)
{
    return (string8){str,size};
}

void str8_print(string8 str)
{
    fwrite(str.bytes, 1, str.size, stderr);
}

string8 str8_fmt(Arenka* arena, char* fmt, ...)
{
    enum {BUFFER_SIZE = 1024};
    u8 buf[BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    int num_chars_wo_null_byte = vsnprintf((char*)buf, BUFFER_SIZE, fmt, args);
    va_end(args);

    if ((size_t)num_chars_wo_null_byte >= sizeof(buf)) {
        num_chars_wo_null_byte = BUFFER_SIZE-1;
    }

    u8* bytes = (u8*)arenka_get_piece(arena, num_chars_wo_null_byte);
    memcpy(bytes, buf, num_chars_wo_null_byte);

    return (string8){bytes, num_chars_wo_null_byte};
}


// global Arenka scratch_arena;
// int main(void)
// {
//     elog_s("PROGRAM INIT...");
//     scratch_arena = arenka_map(MB(64));
//     if (scratch_arena.addr_start == NULL) {
//         elog_abort("memory mapping failed");
//     }
//
//     string8 s1 = str8_fmt(&scratch_arena, "first shit %d", 1);  
//     string8 s2 = str8_fmt(&scratch_arena, "second shit %d", 5);  
//     str8_print(s1);
//     str8_print(s2);
//
//     arenka_unmap(&scratch_arena);
//     return 0;
// }
