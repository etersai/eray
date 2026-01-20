#include <math.h>
#include <stdio.h>

#define eray_norm(value, min, max) (((value) - (min)) / ((max) - (min)))
#define eray_lerp(norm, min, max) (((max) - (min)) * (norm) + (min))
// MAP is the combianation of norm and lerp. (MAP ONE RANGE TO ANOTHER ONE)
#define eray_min(a, b) ((a) < (b) ? (a) : (b))
#define eray_max(a, b) ((a) > (b) ? (a) : (b))
#define eray_clamp(val, lo, hi) (eray_min(eray_max((val), (lo)), (hi)))
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

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
static void vecf4_print(vecf4 vec);
static void vecf3_printf(vecf3 vec);
// COMMON
static inline vecf4 matf4x4_mul_vecf4(const matf4x4* m, vecf4 v);
// VECTORS
static inline float vecf3_len(vecf3 vec);
static inline float vecf3_len_no_sqrt(vecf3 vec);
static inline vecf3 vecf3_add(vecf3 a, vecf3 b);
static inline vecf3 vecf3_sub(vecf3 a, vecf3 b);
static inline vecf3 vecf3_scale(float factor, vecf3 vec);
static inline vecf3 vecf3_cross(vecf3 a, vecf3 b);
static inline float vecf3_dot(vecf3 a, vecf3 b);
static inline vecf3 vecf3_norm(vecf3 vec);
// MATRICES
static inline void matf4x4_I(matf4x4* matrix);
static inline void matf4x4_mul(matf4x4* result, const matf4x4* a, const matf4x4* b);
static inline void matf4x4_scale_uniform(matf4x4* m, float x, float y, float z);

// COMMON
static inline vecf4 matf4x4_mul_vecf4(const matf4x4* m, vecf4 v)
{
    vecf4 result;
    result.x = m->col1.x * v.x + m->col2.x * v.y + m->col3.x * v.z + m->col4.x * v.w;
    result.y = m->col1.y * v.x + m->col2.y * v.y + m->col3.y * v.z + m->col4.y * v.w;
    result.z = m->col1.z * v.x + m->col2.z * v.y + m->col3.z * v.z + m->col4.z * v.w;
    result.w = m->col1.w * v.x + m->col2.w * v.y + m->col3.w * v.z + m->col4.w * v.w;
    return result;
}

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
static inline float vecf3_len_no_sqrt(vecf3 vec) 
{ 
    return vecf3_dot(vec, vec);
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

static inline void matf4x4_scale_set_uniform(matf4x4* m, float x, float y, float z)
{
    m->col1 = (vecf4){x,    0.0f, 0.0f, 0.0f};
    m->col2 = (vecf4){0.0f, y,    0.0f, 0.0f};
    m->col3 = (vecf4){0.0f, 0.0f, z,    0.0f};
    m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
}

static inline void matf4x4_mul(matf4x4* result, const matf4x4* a, const matf4x4* b)
{
    matf4x4 m;

    m.col1.x = a->col1.x * b->col1.x + a->col2.x * b->col1.y + a->col3.x * b->col1.z + a->col4.x * b->col1.w; 
    m.col2.x = a->col1.x * b->col2.x + a->col2.x * b->col2.y + a->col3.x * b->col2.z + a->col4.x * b->col2.w;
    m.col3.x = a->col1.x * b->col3.x + a->col2.x * b->col3.y + a->col3.x * b->col3.z + a->col4.x * b->col3.w;
    m.col4.x = a->col1.x * b->col4.x + a->col2.x * b->col4.y + a->col3.x * b->col4.z + a->col4.x * b->col4.w; 

    m.col1.y = a->col1.y * b->col1.x + a->col2.y * b->col1.y + a->col3.y * b->col1.z + a->col4.y * b->col1.w;
    m.col2.y = a->col1.y * b->col2.x + a->col2.y * b->col2.y + a->col3.y * b->col2.z + a->col4.y * b->col2.w;
    m.col3.y = a->col1.y * b->col3.x + a->col2.y * b->col3.y + a->col3.y * b->col3.z + a->col4.y * b->col3.w;
    m.col4.y = a->col1.y * b->col4.x + a->col2.y * b->col4.y + a->col3.y * b->col4.z + a->col4.y * b->col4.w;

    m.col1.z = a->col1.z * b->col1.x + a->col2.z * b->col1.y + a->col3.z * b->col1.z + a->col4.z * b->col1.w;
    m.col2.z = a->col1.z * b->col2.x + a->col2.z * b->col2.y + a->col3.z * b->col2.z + a->col4.z * b->col2.w;
    m.col3.z = a->col1.z * b->col3.x + a->col2.z * b->col3.y + a->col3.z * b->col3.z + a->col4.z * b->col3.w;
    m.col4.z = a->col1.z * b->col4.x + a->col2.z * b->col4.y + a->col3.z * b->col4.z + a->col4.z * b->col4.w;

    m.col1.w = a->col1.w * b->col1.x + a->col2.w * b->col1.y + a->col3.w * b->col1.z + a->col4.w * b->col1.w;
    m.col2.w = a->col1.w * b->col2.x + a->col2.w * b->col2.y + a->col3.w * b->col2.z + a->col4.w * b->col2.w;
    m.col3.w = a->col1.w * b->col3.x + a->col2.w * b->col3.y + a->col3.w * b->col3.z + a->col4.w * b->col3.w;
    m.col4.w = a->col1.w * b->col4.x + a->col2.w * b->col4.y + a->col3.w * b->col4.z + a->col4.w * b->col4.w;

    *result = m;
}


// DEBUG
#define VECF4_PRINT_FORMAT "[%f, %f, %f, %f]\n"
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
static void vecf4_print(vecf4 vec)
{
    fprintf(stdout, VECF4_PRINT_FORMAT, vec.x, vec.y, vec.z, vec.w);
    fflush(stdout);
}
static void vecf3_print(vecf3 vec)
{
    fprintf(stdout, VECF3_PRINT_FORMAT, vec.x, vec.y, vec.z);
    fflush(stdout);
}
