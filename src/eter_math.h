// Public Domain.
#ifndef ETER_MATH_H_
#define ETER_MATH_H_

#include <math.h>

// TODO
// -Add array based vectors.
// -Make vec impl stb style lib.
// -Add strip prefix thingy.

/***************************/
/* Basic macros/operations */
/***************************/
#define ETER_MAX(a,b) ((a) > (b) ? (a) : (b))
#define ETER_ARRLEN(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define ETER_SQUARE(x) ((x)*(x))

/************************/
/* Struct Based Vectors */
/************************/
/* Types */
typedef struct {
    int x;
    int y;
} ivec2;

typedef struct {
    int x;
    int y;
    int z;
} ivec3;

typedef struct {
    float x;
    float y;
} fvec2;

typedef struct {
    float x;
    float y;
    float z;
} fvec3;

/* Constructors */
#define IVEC2(x,y) ((ivec2){(x),(y)})
#define IVEC3(x,y,z) ((ivec3){(x),(y),(z)})
#define FVEC2(x,y) ((fvec2){(x),(y)})
#define FVEC3(x,y,z) ((fvec3){(x),(y),(z)}) 

/* Operations */
static inline ivec2 ivec2_add(ivec2 v1, ivec2 v2) { return (ivec2){v1.x + v2.x, v1.y + v2.y}; }
static inline ivec2 ivec2_sub(ivec2 v1, ivec2 v2) { return (ivec2){v1.x - v2.x, v1.y - v2.y}; }
static inline ivec2 ivec2_mul(ivec2 v1, ivec2 v2) { return (ivec2){v1.x * v2.x, v1.y * v2.y}; }
static inline ivec2 ivec2_div(ivec2 v1, ivec2 v2) { return (ivec2){v1.x / v2.x, v1.y / v2.y}; }
static inline ivec2 ivec2_scale(ivec2 v, int s) { return (ivec2){v.x * s, v.y * s}; }
static inline int ivec2_dot(ivec2 v1, ivec2 v2) { return v1.x*v2.x + v1.y*v2.y; }
static inline float ivec2_len(ivec2 v) { return sqrtf((float)ivec2_dot(v, v)); }
static inline int ivec2_len2(ivec2 v) { return ivec2_dot(v, v); }
static inline float ivec2_dist(ivec2 a, ivec2 b) { return ivec2_len(ivec2_sub(a, b)); }
static inline int ivec2_dist2(ivec2 a, ivec2 b) { return ivec2_len2(ivec2_sub(a, b)); }


static inline ivec3 ivec3_add(ivec3 v1, ivec3 v2) { return (ivec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline ivec3 ivec3_sub(ivec3 v1, ivec3 v2) { return (ivec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline ivec3 ivec3_mul(ivec3 v1, ivec3 v2) { return (ivec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline ivec3 ivec3_div(ivec3 v1, ivec3 v2) { return (ivec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline ivec3 ivec3_scale(ivec3 v, int s) { return (ivec3){v.x * s, v.y * s, v.z * s}; }
static inline int ivec3_dot(ivec3 v1, ivec3 v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
static inline float ivec3_len(ivec3 v) { return sqrtf((float)ivec3_dot(v, v)); }
static inline int ivec3_len2(ivec3 v) { return ivec3_dot(v, v); }
static inline float ivec3_dist(ivec3 a, ivec3 b) { return ivec3_len(ivec3_sub(a, b)); }
static inline int ivec3_dist2(ivec3 a, ivec3 b) { return ivec3_len2(ivec3_sub(a, b)); }


static inline fvec2 fvec2_add(fvec2 v1, fvec2 v2) { return (fvec2){v1.x + v2.x, v1.y + v2.y}; }
static inline fvec2 fvec2_sub(fvec2 v1, fvec2 v2) { return (fvec2){v1.x - v2.x, v1.y - v2.y}; }
static inline fvec2 fvec2_mul(fvec2 v1, fvec2 v2) { return (fvec2){v1.x * v2.x, v1.y * v2.y}; }
static inline fvec2 fvec2_div(fvec2 v1, fvec2 v2) { return (fvec2){v1.x / v2.x, v1.y / v2.y}; }
static inline fvec2 fvec2_scale(fvec2 v, float s) { return (fvec2){v.x * s, v.y * s}; }
static inline float fvec2_dot(fvec2 v1, fvec2 v2) { return v1.x*v2.x + v1.y*v2.y; }
static inline float fvec2_len(fvec2 v) { return sqrtf(fvec2_dot(v, v)); }
static inline float fvec2_len2(fvec2 v) { return fvec2_dot(v, v); }
static inline float fvec2_dist(fvec2 a, fvec2 b) { return fvec2_len(fvec2_sub(a, b)); }
static inline float fvec2_dist2(fvec2 a, fvec2 b) { return fvec2_len2(fvec2_sub(a, b)); }
static inline void fvec2_norm(fvec2* v) { float len = fvec2_len(*v); if (len > 0) { len = 1 / len; v->x *= len; v->y *= len; }}
static inline fvec2 fvec2_normalized(fvec2 v) { float len = fvec2_len(v); if (len > 0) { len = 1 / len; return FVEC2(v.x*len, v.y*len);} return FVEC2(0.0f, 0.0f);}


static inline fvec3 fvec3_add(fvec3 v1, fvec3 v2) { return (fvec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline fvec3 fvec3_sub(fvec3 v1, fvec3 v2) { return (fvec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline fvec3 fvec3_mul(fvec3 v1, fvec3 v2) { return (fvec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline fvec3 fvec3_div(fvec3 v1, fvec3 v2) { return (fvec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline fvec3 fvec3_scale(fvec3 v, float s) { return (fvec3){v.x * s, v.y * s, v.z * s}; }
static inline float fvec3_dot(fvec3 v1, fvec3 v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
static inline float fvec3_len(fvec3 v) { return sqrtf(fvec3_dot(v, v)); }
static inline float fvec3_len2(fvec3 v) { return fvec3_dot(v, v); }
static inline float fvec3_dist(fvec3 a, fvec3 b) { return fvec3_len(fvec3_sub(a, b)); }
static inline float fvec3_dist2(fvec3 a, fvec3 b) { return fvec3_len2(fvec3_sub(a, b)); }
static inline void fvec3_norm(fvec3* v) { float len = fvec3_len(*v); if (len > 0) { len = 1 / len; v->x *= len; v->y *= len; v->z *= len;}}
static inline fvec3 fvec3_normalized(fvec3 v) { float len = fvec3_len(v); if (len > 0) { len = 1 / len; return FVEC3(v.x*len, v.y*len, v.z*len);} return FVEC3(0.0f, 0.0f, 0.0f);}
static inline fvec3 fvec3_cross(fvec3 a, fvec3 b) {
    float cx = a.y*b.z - a.z*b.y;
    float cy = a.z*b.x - a.x*b.z;
    float cz = a.x*b.y - a.y*b.x;
    return FVEC3(cx,cy,cz);
}

/* Debug */
#define IVEC2_PRINT(v) do {                    \
    printf("[IVEC2: %d, %d]\n", (v).x, (v).y); \
} while (0)

#define IVEC3_PRINT(v) do {                    \
    printf("[IVEC3: %d, %d, %d]\n", (v).x, (v).y, (v).z); \
} while (0)

#define FVEC2_PRINT(v) do {                    \
    printf("[FVEC2: %f, %f]\n", (v).x, (v).y); \
} while (0)

#define FVEC3_PRINT(v) do {                    \
    printf("[FVEC3: %f, %f, %f]\n", (v).x, (v).y, (v).z); \
} while (0)

#endif /* ETER_MATH_H_ [etesai 2025]*/
