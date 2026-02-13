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

internal string8 str8(u8* str, u64 size)
{
    return (string8){str,size};
}

internal void str8_print(string8 str)
{
    fwrite(str.bytes, 1, str.size, stderr);
}

#define str8_lit(str) str8((u8*)(str), sizeof(str) - 1)

// log_print_n_flush("[FPS: %d | DELTA TIME: %f]\n", frames, delta_time);
// log_print_n_flush("[CAMERA POS] X: %f, Y: %f, Z: %f\n", program_ctx.camera.pos.x, program_ctx.camera.pos.y, program_ctx.camera.pos.z);
// log_print_n_flush("[CAMERA DIR] X: %f, Y: %f, Z: %f\n", program_ctx.camera.orientation.x, program_ctx.camera.orientation.y, program_ctx.camera.orientation.z);

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

global Arenka string_arena;

int main(void)
{
    string_arena = arenka_map(MB(10));

    elog_s("PROGRAM INIT...");
    string8 ret = str8_format("Niech zacznie padac %d, %.2f\n", 14, 14.4f);
    str8_print(ret);    
    free(ret.bytes);

    arenka_unmap(&a);
    return 0;
}
