#include "elog.h"
#include <stdint.h>

// base types. credit RadDebugger.
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

typedef global static;

typedef struct {
    u8 *str;    
    u64 size;
} string8;

typedef struct {
    enum {
        CIRCLE = 0,
        SQUARE,
        TRIANGLE,
    } shape_type;
    f32 data[2];
} Shape;

static const f64 pi = 3.14159265359;


int main(void)
{
    elog_zu(sizeof(Shape)); 
    Shape s;
    
    s.shape_type = CIRCLE;
    s.data[0] = 5.0f;

    switch(s.shape_type) {
        case CIRCLE: elog_s("circle"); break;
        case SQUARE: elog_s("square"); break;
        case TRIANGLE: elog_s("triangle"); break;
        default: abort();
    }

    return 0;
}
