#ifndef LA_MATH_H_
#define LA_MATH_H_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define la_norm(value, min, max) (((value) - (min)) / ((max) - (min)))
#define la_lerp(norm, min, max) (((max) - (min)) * (norm) + (min))
#define la_min(a, b) ((a) < (b) ? (a) : (b))
#define la_max(a, b) ((a) > (b) ? (a) : (b))
#define la_clamp(val, lo, hi) (la_min(la_max((val), (lo)), (hi)))
// this trick is amazing! flipped_value = max + min - original_value
//TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
// NOTE: MAP is the combianation of norm and lerp. (MAP ONE RANGE TO ANOTHER ONE)
//#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

#define la_expect(var) do {                                                \
    if (!(var)) {                                                          \
        fprintf(stderr, "Perma assert: %s:%d: assertion '%s' failed\n",    \
        __FILE__, __LINE__, #var);                                         \
        abort();                                                           \
    }                                                                      \
} while (0)

#define la_unreachable() do {                                                  \
    fprintf(stderr, "Unreachable code hit at %s:%d\n", __FILE__, __LINE__);    \
    abort();                                                                   \
} while (0)

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
    vecf3 col1;
    vecf3 col2;
    vecf3 col3;
} matf3x3;

typedef struct {
    vecf4 col1;
    vecf4 col2;
    vecf4 col3;
    vecf4 col4;
} matf4x4;

// SHORTCUTS
#define VECF3_ZERO (vecf3){0.0f, 0.0f, 0.0f}
#define VECF4_ZERO (vecf4){0.0f, 0.0f, 0.0f, 0.0f}

// DEBUG
static void matf4x4_print(const matf4x4* matrix);
static void matf3x3_print(const matf3x3* matrix);
static void vecf4_print(vecf4 vec);
static void vecf3_printf(vecf3 vec);
static void log_print_n_flush(const char* format, ...);
static void log_print_prefix(const char* prefix, const char* format, ...);
// CONVERSIONS
static inline float rad_to_deg(float radians);
static inline float deg_to_rad(float degrees);
// COMMON
static inline void lamath_projection_matrix(matf4x4* m, float fov, float aspect, float near, float far);
static inline void lamath_lookat_matrix(matf4x4* lookat, vecf3 eye, vecf3 center, vecf3 up);
static inline vecf4 matf4x4_mul_vecf4(const matf4x4* m, vecf4 v);
// VECTORS
static inline float vecf3_len(vecf3 vec);
static inline float vecf3_len_no_sqrt(vecf3 vec);
static inline vecf3 vecf3_add(vecf3 a, vecf3 b);
static inline void vecf3_apply_add(vecf3* a, vecf3 b);
static inline vecf3 vecf3_sub(vecf3 a, vecf3 b);
static inline vecf3 vecf3_scale(float factor, vecf3 vec);
static inline vecf3 vecf3_cross(vecf3 a, vecf3 b);
static inline float vecf3_dot(vecf3 a, vecf3 b);
static inline vecf3 vecf3_norm(vecf3 vec);
// MATRICES
static inline void matf4x4_I(matf4x4* matrix);
static inline matf4x4 matf4x4_I_give(void);
static inline void matf4x4_rot_x(matf4x4* m, float angle);
static inline void matf4x4_rot_y(matf4x4* m, float angle);
static inline void matf4x4_rot_z(matf4x4* m, float angle);
static inline void matf4x4_mul(matf4x4* result, const matf4x4* a, const matf4x4* b);
static inline void matf4x4_scale_set(matf4x4* m, float x, float y, float z);
static inline matf4x4 matf4x4_translate_give(vecf3 translate);

/*
** IMPLEMENTATION
*/
// CONVERSIONS
static inline float rad_to_deg(float radians) { return 180.0f/M_PI*radians; }
static inline float deg_to_rad(float degrees) { return M_PI/180.0f*degrees; }

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

static inline void lamath_projection_matrix(matf4x4* m, float fov, float aspect, float near, float far)
{ // TODO: UNDERSTAND THIS!
    float f = 1.0f / tanf(fov * 0.5f * M_PI / 180.0f);

    matf4x4 proj = {0};

    proj.col1.x = f / aspect;
    proj.col2.y = f;
    proj.col3.z = -(far + near) / (far - near);
    proj.col3.w = -1.0f;
    proj.col4.z = -(2.0f * far * near) / (far - near);

    *m = proj;
}

static inline void lamath_lookat_matrix(matf4x4* lookat, vecf3 eye, vecf3 center, vecf3 up)
{ 
    vecf3 f = vecf3_norm(vecf3_sub(center, eye));
    vecf3 s = vecf3_norm(vecf3_cross(f, up));
    vecf3 t = vecf3_cross(s, f);

    lookat->col1.x = s.x;
    lookat->col1.y = t.x;
    lookat->col1.z = -f.x;
    lookat->col1.w = 0.0f;
    
    lookat->col2.x = s.y; 
    lookat->col2.y = t.y;
    lookat->col2.z = -f.y;
    lookat->col2.w = 0.0f;
    
    lookat->col3.x = s.z;
    lookat->col3.y = t.z;
    lookat->col3.z = -f.z;
    lookat->col3.w = 0.0f;
    
    lookat->col4.x = 0.0f;
    lookat->col4.y = 0.0f;
    lookat->col4.z = 0.0f;
    lookat->col4.w = 1.0f;

    lookat->col4.x = -vecf3_dot(s, eye);
    lookat->col4.y = -vecf3_dot(t, eye);
    lookat->col4.z =  vecf3_dot(f, eye);
    lookat->col4.w = 1.0f;
}

// VECTORS
static inline vecf3 vecf3_add(vecf3 a, vecf3 b)
{
    return (vecf3){a.x+b.x, a.y+b.y, a.z+b.z};
}
static inline void vecf3_apply_add(vecf3* a, vecf3 b)
{
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
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

static inline matf4x4 matf4x4_I_give(void)
{
    return (matf4x4){.col1.x = 1.0f, .col2.x = 0.0f, .col3.x = 0.0f, .col4.x = 0.0f,
                     .col1.y = 0.0f, .col2.y = 1.0f, .col3.y = 0.0f, .col4.y = 0.0f,
                     .col1.z = 0.0f, .col2.z = 0.0f, .col3.z = 1.0f, .col4.z = 0.0f,
                     .col1.w = 0.0f, .col2.w = 0.0f, .col3.w = 0.0f, .col4.w = 1.0f,};
}

static inline void matf4x4_scale_set(matf4x4* m, float x, float y, float z)
{
    m->col1 = (vecf4){x,    0.0f, 0.0f, 0.0f};
    m->col2 = (vecf4){0.0f, y,    0.0f, 0.0f};
    m->col3 = (vecf4){0.0f, 0.0f, z,    0.0f};
    m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
}


static inline matf4x4 matf4x4_translate_give(vecf3 translate)
{
    return (matf4x4){.col1.x = 1.0f, .col2.x = 0.0f, .col3.x = 0.0f, .col4.x = translate.x,
                     .col1.y = 0.0f, .col2.y = 1.0f, .col3.y = 0.0f, .col4.y = translate.y,
                     .col1.z = 0.0f, .col2.z = 0.0f, .col3.z = 1.0f, .col4.z = translate.z,
                     .col1.w = 0.0f, .col2.w = 0.0f, .col3.w = 0.0f, .col4.w = 1.0f,};
}

static inline void matf4x4_rot_x(matf4x4* m, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);

    m->col1.x = 1.0f;
    m->col1.y = 0.0f;
    m->col1.z = 0.0f;
    m->col1.w = 0.0f;

    m->col2.x = 0.0f;
    m->col2.y = c;
    m->col2.z = s;
    m->col2.w = 0.0f;

    m->col3.x = 0.0f; 
    m->col3.y = -s;
    m->col3.z = c; 
    m->col3.w = 0.0f; 
    
    m->col4.x = 0.0f;
    m->col4.y = 0.0f;
    m->col4.z = 0.0f;
    m->col4.w = 1.0f;
}

static inline void matf4x4_rot_y(matf4x4* m, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    
    m->col1.x = c;
    m->col1.y = 0.0f;
    m->col1.z = -s;
    m->col1.w = 0.0f;

    m->col2.x = 0.0f;
    m->col2.y = 1.0f;
    m->col2.z = 0.0f;
    m->col2.w = 0.0f;

    m->col3.x = s; 
    m->col3.y = 0.0f;
    m->col3.z = c; 
    m->col3.w = 0.0f; 
    
    m->col4.x = 0.0f;
    m->col4.y = 0.0f;
    m->col4.z = 0.0f;
    m->col4.w = 1.0f;
}

static inline void matf4x4_rot_z(matf4x4* m, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    
    m->col1.x = c;
    m->col1.y = s;
    m->col1.z = 0.0f;
    m->col1.w = 0.0f;

    m->col2.x = -s;
    m->col2.y = c;
    m->col2.z = 0.0f;
    m->col2.w = 0.0f;

    m->col3.x = 0.0f; 
    m->col3.y = 0.0f;
    m->col3.z = 1.0f; 
    m->col3.w = 0.0f; 
    
    m->col4.x = 0.0f;
    m->col4.y = 0.0f;
    m->col4.z = 0.0f;
    m->col4.w = 1.0f;
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

// DEBUG MATH
#define VECF4_PRINT_FORMAT "[%f, %f, %f, %f]\n"
#define VECF3_PRINT_FORMAT "[%f, %f, %f]\n"
#define MATRIX3X3_PRINT_FORMAT "[%.2f, %.2f, %.2f]\n"
#define MATRIX4X4_PRINT_FORMAT "[%.2f, %.2f, %.2f, %.2f]\n"
static void matf4x4_print(const matf4x4* matrix)
{
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.x, matrix->col2.x, matrix->col3.x, matrix->col4.x); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.y, matrix->col2.y, matrix->col3.y, matrix->col4.y); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.z, matrix->col2.z, matrix->col3.z, matrix->col4.z); 
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->col1.w, matrix->col2.w, matrix->col3.w, matrix->col4.w); 
    fflush(stdout);
}
static void matf3x3_print(const matf3x3* matrix)
{
    fprintf(stdout, MATRIX3X3_PRINT_FORMAT, matrix->col1.x, matrix->col2.x, matrix->col3.x); 
    fprintf(stdout, MATRIX3X3_PRINT_FORMAT, matrix->col1.y, matrix->col2.y, matrix->col3.y); 
    fprintf(stdout, MATRIX3X3_PRINT_FORMAT, matrix->col1.z, matrix->col2.z, matrix->col3.z); 
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

//
// LOG 
//
#define LAMATH_LOG_MAX_PREFIX 64
#define LOG_MATRIX(matrix) do { matf4x4_print(&(matrix)); } while(0)
#define LOG_VEC(vector) do { vecf3_print((vector)); } while(0)
static void log_print_prefix(const char* prefix, const char* format, ...)
{
    la_expect(prefix);
    la_expect(format);

    int idx = 0;
    char buffer[LAMATH_LOG_MAX_PREFIX] = {0};
    while (*prefix) {
        if (idx >= LAMATH_LOG_MAX_PREFIX) {
            break; 
        }
        if (*prefix >= 'a' && *prefix <= 'z') {
            buffer[idx] = *prefix^(1 << 5); // toggle case bit with xor voodoo.
        } 
        else
        {
            buffer[idx] = *prefix;
        }
        prefix++; idx++;
    } 

    FILE* out_target = stderr; 
    fprintf(out_target, "[%s]: ", buffer);
    va_list args;
    va_start(args, format);
    vfprintf(out_target, format, args);
    va_end(args);
}

static void log_print_n_flush(const char* format, ...)
{
    FILE* out_target = stdout; 
    va_list args;
    va_start(args, format);
    vfprintf(out_target, format, args);
    va_end(args);
    fflush(out_target);
}
#endif /* LA_MATH_H_ */
