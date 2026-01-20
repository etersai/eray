#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define eray_norm(value, min, max) (((value) - (min)) / ((max) - (min)))
#define eray_lerp(norm, min, max) (((max) - (min)) * (norm) + (min))
// MAP is the combianation of norm and lerp. (MAP ONE RANGE TO ANOTHER ONE)
#define eray_min(a, b) ((a) < (b) ? (a) : (b))
#define eray_max(a, b) ((a) > (b) ? (a) : (b))
#define eray_clamp(val, lo, hi) (eray_min(eray_max((val), (lo)), (hi)))
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

#define expect(var) do {                                                \
    if (!(var)) {                                                       \
        fprintf(stderr, "Perma assert: %s:%d: assertion '%s' failed\n", \
        __FILE__, __LINE__, #var);                                      \
        abort();                                                        \
    }                                                                   \
} while (0)

#define unreachable() do {                                                  \
    fprintf(stderr, "Unreachable code hit at %s:%d\n", __FILE__, __LINE__); \
    abort();                                                                \
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
    vecf4 col1;
    vecf4 col2;
    vecf4 col3;
    vecf4 col4;
} matf4x4;

typedef enum {
    AXIS_X = 0,
    AXIS_Y = 0,
    AXIS_Z = 0,
} axis_t;

// DEBUG
static void matf4x4_print(const matf4x4* matrix);
static void vecf4_print(vecf4 vec);
static void vecf3_printf(vecf3 vec);
// CONVERSIONS
static inline float rad_to_deg(float radians);
static inline float deg_to_rad(float degrees);
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
static inline void matf4x4_rot_x(matf4x4* m, float angle);
static inline void matf4x4_rot_y(matf4x4* m, float angle);
static inline void matf4x4_rot_z(matf4x4* m, float angle);
static inline void matf4x4_mul(matf4x4* result, const matf4x4* a, const matf4x4* b);
static inline void matf4x4_scale_uniform(matf4x4* m, float x, float y, float z);
static inline void matf4x4_scale_one_axis(matf4x4* m, float scale, axis_t axis);
static inline void matf4x4_translate_make(matf4x4* m, vecf3 translate);

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

static inline void matf4x4_scale_uniform(matf4x4* m, float x, float y, float z)
{
    m->col1 = (vecf4){x,    0.0f, 0.0f, 0.0f};
    m->col2 = (vecf4){0.0f, y,    0.0f, 0.0f};
    m->col3 = (vecf4){0.0f, 0.0f, z,    0.0f};
    m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
}

static inline void matf4x4_scale_one_axis(matf4x4* m, float scale, axis_t axis)
{
    if (axis == AXIS_X) {
        m->col1 = (vecf4){scale,0.0f, 0.0f, 0.0f};
        m->col2 = (vecf4){0.0f, 1.0f, 0.0f, 0.0f};
        m->col3 = (vecf4){0.0f, 0.0f, 1.0f, 0.0f};
        m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
    }
    else if (axis == AXIS_Y)
    {
        m->col1 = (vecf4){1.0f, 0.0f, 0.0f, 0.0f};
        m->col2 = (vecf4){0.0f, scale, 0.0f, 0.0f};
        m->col3 = (vecf4){0.0f, 0.0f, 1.0f, 0.0f};
        m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
    }
    else if (axis == AXIS_Z)
    {
        m->col1 = (vecf4){1.0f, 0.0f, 0.0f, 0.0f};
        m->col2 = (vecf4){0.0f, 1.0f, 0.0f, 0.0f};
        m->col3 = (vecf4){0.0f, 0.0f, scale, 0.0f};
        m->col4 = (vecf4){0.0f, 0.0f, 0.0f, 1.0f};
    }
    else
    {
        unreachable();
    }
}


static inline void matf4x4_translate_make(matf4x4* m, vecf3 translate)
{
    matf4x4_I(m);
    m->col4 = (vecf4){translate.x, translate.y, translate.z, 1.0f};
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

//
// LOG 
//
#define LAMATH_LOG_MAX_PREFIX 64
void log_print_beautify(const char* prefix, const char* format, ...)
{
    expect(prefix);
    expect(format);

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

void log_print_n_flush(const char* format, ...)
{
    FILE* out_target = stdout; 
    va_list args;
    va_start(args, format);
    vfprintf(out_target, format, args);
    va_end(args);
    fflush(out_target);
}


