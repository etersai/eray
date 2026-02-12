#ifndef ERAY_COMMON_H_
#define ERAY_COMMON_H_

#include <stdint.h>

// credit Ryan Fleury
#define global static 
#define internal static
#define local_persist static

// BASE TYPES
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

typedef struct {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} Colorek;

#endif /* ERAY_COMMON_H_ */
