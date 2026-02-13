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

#define global static
#define local_persist static

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
    f64 data[2];
} Shape;

typedef struct {
    enum {
        U_CIRCLE = 0,
        U_SQUARE,
        U_TRIANGLE,
    } shape_type;
    union {
        struct { f64 radius; } circle;
        struct { f64 side; } square;
        struct { f64 base; f64 height; } triangle;
    };
} UShape;

f64 calculate_shape_area(Shape shape)
{
    local_persist const f64 pi = 3.14159265358979323846;
    if (shape.shape_type == CIRCLE) {
        f64 r = shape.data[0];   
        return (pi*r*r);
    }
    else if (shape.shape_type == SQUARE) {
        f64 s = shape.data[0];
        return (s*s);
    }
    else if (shape.shape_type == TRIANGLE) {
        f64 b = shape.data[0]; 
        f64 hb = shape.data[1];
        return (hb*b/2);
    }
    return 0.0;
}

int main(void)
{
    Shape s = {.shape_type = CIRCLE};
    s.data[0] = 5.0;
    f64 area = calculate_shape_area(s);
    elog_f(area);
    return 0;
}
