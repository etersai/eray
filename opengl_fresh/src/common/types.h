#ifndef ERAY_TYPES_H_
#define ERAY_TYPES_H_

#include <stdint.h>
#include <assert.h>

#define unused(var) ((void)(var))
#define perma_assert(x) do { if (!x) { fprintf(stderr,"Perma assertion: %s, %s:%d\n", #x, __FILE__, __LINE__); abort(); }} while (0)
#ifndef arrlen
#define arrlen(arr) (sizeof((arr)) / sizeof((arr)[0]))
#endif /*arrlen*/
#ifndef unreachable
#define unreachable() do { \
    fprintf(stderr, "Unreachable code path reached! %s:%d in function: %s\n", __FILE__, __LINE__, __func__); \
    abort(); \
} while (0)
#endif /*unreachable*/

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
typedef u32      b32;

typedef struct {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} Colorek;

#endif /* ERAY_TYPES_H_ */
