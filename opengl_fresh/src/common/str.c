#include "str.h"
#include "string.h" // memcpy
#include "types.h"
#include "arena.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

string8 str8(u8* str, u64 size)
{
    return (string8){str,size};
}

string8 str8_cstr(const char* cstr)
{
    return (string8){(u8*)cstr, strlen(cstr)};
}

string8 str8_from_cstr(Arenka* arenka, const char* cstr)
{
    
}

void str8_print(string8 str)
{
    fwrite(str.bytes, 1, str.size, stderr);
    putchar('\n');
    fprintf(stderr, "str8 size: %zu", str.size);
    putchar('\n');
    fflush(stderr);
}

bool str8_cmp_cstr(string8 str, const char* cstr)
{
    u64 len = strlen(cstr);
    if (len != str.size) 
    {
        return false;
    }
    if (memcmp(cstr, str.bytes, len) != 0)
    {
        return false;
    }

    return true;
}

bool str8_get_extension_after_dot(string8 str, string8* extension)
{
    assert(extension);
    for (u64 i = 0; i < str.size; i++) {
        if (str.bytes[i] == '.') {
            extension->bytes = str.bytes+(i+1);
            extension->size = str.size-(i+1);
            return true;
        }
    }
    return false;
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
