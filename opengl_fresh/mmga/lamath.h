#include <math.h>
#include <stdio.h>

typedef struct {
    float x;
    float y;
    float z;
} vecf3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vecf4;

typedef struct {
    vecf4 col1;
    vecf4 col2;
    vecf4 col3;
    vecf4 col4;
} matf4x4;

// DEBUG
static void matf4x4_print(const matf4x4* matrix);
static void vecf3_printf(vecf3 vec);
// VECTORS
static inline float vecf3_len(vecf3 vec);
static inline vecf3 vecf3_add(vecf3 a, vecf3 b);
static inline vecf3 vecf3_sub(vecf3 a, vecf3 b);
static inline vecf3 vecf3_scale(float factor, vecf3 vec);
static inline vecf3 vecf3_cross(vecf3 a, vecf3 b);
static inline float vecf3_dot(vecf3 a, vecf3 b);
static inline vecf3 vecf3_norm(vecf3 vec);
// MATRICES
static inline void matf4x4_I(matf4x4* matrix);
static inline void matf4x4_mul(const matf4x4* a, const matf4x4* b, matf4x4* result);


// VECTORS
static inline vecf3 vecf3_add(vecf3 a, vecf3 b)
{
    return (vecf3){a.x+b.x, a.y+b.y, a.z+b.z};
}
static inline vecf3 vecf3_sub(vecf3 a, vecf3 b)
{
    return (vecf3){a.x-b.x, a.y-b.y, a.z-b.z};
}
static inline vecf3 vecf3_scale(float factor, vecf3 vec)
{
    return (vecf3){factor*vec.x, factor*vec.y, factor*vec.z};
}
static inline vecf3 vecf3_norm(vecf3 vec)
{
    float len = vecf3_len(vec);
    if (len != 0.0f) {
        return (vecf3){vec.x/len, vec.y/len, vec.z/len};
    }
    return (vecf3){0.0f, 0.0f, 0.0f};
}
static inline float vecf3_dot(vecf3 a, vecf3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static inline float vecf3_len(vecf3 vec) 
{ 
    return sqrtf(vecf3_dot(vec, vec));
}
static inline vecf3 vecf3_cross(vecf3 a, vecf3 b)
{
    return (vecf3){a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x};
}

// MATRICES
static inline void matf4x4_I(matf4x4* matrix)
{
    matrix->col1.x = 1.0f;
    matrix->col1.y = 0.0f;
    matrix->col1.z = 0.0f;
    matrix->col1.w = 0.0f;

    matrix->col2.x = 0.0f;
    matrix->col2.y = 1.0f;
    matrix->col2.z = 0.0f;
    matrix->col2.w = 0.0f;

    matrix->col3.x = 0.0f;
    matrix->col3.y = 0.0f;
    matrix->col3.z = 1.0f;
    matrix->col3.w = 0.0f;

    matrix->col4.x = 0.0f;
    matrix->col4.y = 0.0f;
    matrix->col4.z = 0.0f;
    matrix->col4.w = 1.0f;
}

static inline void matf4x4_mul(const matf4x4* a, const matf4x4* b, matf4x4* result)
{

}

// DEBUG
#define VECF3_PRINT_FORMAT "[%f, %f, %f]\n"
#define MATRIX4X4_PRINT_FORMAT "[%.2f, %.2f, %.2f, %.2f]\n"
static void matf4x4_print(const matf4x4* matrix)
{
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.x, matrix->col2.x, matrix->col3.x, matrix->col4.x); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.y, matrix->col2.y, matrix->col3.y, matrix->col4.y); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.z, matrix->col2.z, matrix->col3.z, matrix->col4.z); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.w, matrix->col2.w, matrix->col3.w, matrix->col4.w); 
    fflush(stdout);
}
static void vecf3_print(vecf3 vec)
{
    fprintf(stdout, VECF3_PRINT_FORMAT, vec.x, vec.y, vec.z);
    fflush(stdout);
}
