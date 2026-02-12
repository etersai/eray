#ifndef ERAY_COMMON_H_
#define ERAY_COMMON_H_

#include <stdint.h>

// credit Ryan Fleury
#define global        static 
#define internal      static
#define local_persist static

// BASE TYPES
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef float    F32;
typedef double   F64;

typedef struct {
    F32 r;
    F32 g;
    F32 b;
    F32 a;
} Colorek;

#endif /* ERAY_COMMON_H_ */
