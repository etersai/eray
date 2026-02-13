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
    fwrite(str.bytes, 1, str.size, stdout);
}

#define str8_lit(str) str8((u8*)(str), sizeof(str) - 1)

// log_print_n_flush("[FPS: %d | DELTA TIME: %f]\n", frames, delta_time);
// log_print_n_flush("[CAMERA POS] X: %f, Y: %f, Z: %f\n", program_ctx.camera.pos.x, program_ctx.camera.pos.y, program_ctx.camera.pos.z);
// log_print_n_flush("[CAMERA DIR] X: %f, Y: %f, Z: %f\n", program_ctx.camera.orientation.x, program_ctx.camera.orientation.y, program_ctx.camera.orientation.z);

internal void str8_format(char* fmt, ...)
{
    u8 buf[KB(1)];
    va_list args;
    va_start(args, fmt);
    int number = vsnprintf(buf, sizeof(buf), fmt, args);
    elog_d(number);
    abort();
    va_end(args);

    elog_s(buf);
}

int main(void)
{
    elog_s("PROGRAM INIT...");
    
    str8_format("asdsadas");

    return 0;
}
