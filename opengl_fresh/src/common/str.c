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

string8 str8_cstr(const char* cstr)
{
    return (string8){(u8*)cstr, strlen(cstr)};
}

void str8_print(string8 str)
{
    fwrite(str.bytes, 1, str.size, stderr);
    putchar('\n');
}

string8 str8_fmt(Arenka* arena, char* fmt, ...)
{
    enum {BUFFER_SIZE = 2048};
    u8 buf[BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    int num_chars_wo_null_byte = vsnprintf((char*)buf, BUFFER_SIZE, fmt, args);
    va_end(args);

    if ((size_t)num_chars_wo_null_byte >= sizeof(buf)) {
        num_chars_wo_null_byte = BUFFER_SIZE-1;
    }

    u8* bytes = (u8*)arenka_get_piece(arena, num_chars_wo_null_byte);
    if (bytes == NULL) {
        return STR8_NULL;
    }
    memcpy(bytes, buf, num_chars_wo_null_byte);

    return (string8){bytes, num_chars_wo_null_byte};
}
