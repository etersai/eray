// Public Domain.
#ifndef ETER_MATH_H_
#define ETER_MATH_H_

#include <math.h>  // sqrt
#include <stdio.h> // debug prints.

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
static inline ivec2 ivec2_add(const ivec2 v1, const ivec2 v2) { return (ivec2){v1.x + v2.x, v1.y + v2.y}; }
static inline ivec2 ivec2_sub(const ivec2 v1, const ivec2 v2) { return (ivec2){v1.x - v2.x, v1.y - v2.y}; }
static inline ivec2 ivec2_mul(const ivec2 v1, const ivec2 v2) { return (ivec2){v1.x * v2.x, v1.y * v2.y}; }
static inline ivec2 ivec2_div(const ivec2 v1, const ivec2 v2) { return (ivec2){v1.x / v2.x, v1.y / v2.y}; }
static inline ivec2 ivec2_scale(const ivec2 v, int s) { return (ivec2){v.x * s, v.y * s}; }
static inline int ivec2_dot(const ivec2 v1, const ivec2 v2) { return v1.x*v2.x + v1.y*v2.y; }
static inline float ivec2_len(const ivec2 v) { return sqrtf((float)ivec2_dot(v, v)); }
static inline int ivec2_len2(const ivec2 v) { return ivec2_dot(v, v); }
static inline float ivec2_dist(const ivec2 a, const ivec2 b) { return ivec2_len(ivec2_sub(a, b)); }
static inline int ivec2_dist2(const ivec2 a, const ivec2 b) { return ivec2_len2(ivec2_sub(a, b)); }


static inline ivec3 ivec3_add(const ivec3 v1, const ivec3 v2) { return (ivec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline ivec3 ivec3_sub(const ivec3 v1, const ivec3 v2) { return (ivec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline ivec3 ivec3_mul(const ivec3 v1, const ivec3 v2) { return (ivec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline ivec3 ivec3_div(const ivec3 v1, const ivec3 v2) { return (ivec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline ivec3 ivec3_scale(const ivec3 v, int s) { return (ivec3){v.x * s, v.y * s, v.z * s}; }
static inline int ivec3_dot(const ivec3 v1, const ivec3 v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
static inline float ivec3_len(const ivec3 v) { return sqrtf((float)ivec3_dot(v, v)); }
static inline int ivec3_len2(const ivec3 v) { return ivec3_dot(v, v); }
static inline float ivec3_dist(const ivec3 a, const ivec3 b) { return ivec3_len(ivec3_sub(a, b)); }
static inline int ivec3_dist2(const ivec3 a, const ivec3 b) { return ivec3_len2(ivec3_sub(a, b)); }


static inline fvec2 fvec2_add(const fvec2 v1, const fvec2 v2) { return (fvec2){v1.x + v2.x, v1.y + v2.y}; }
static inline fvec2 fvec2_sub(const fvec2 v1, const fvec2 v2) { return (fvec2){v1.x - v2.x, v1.y - v2.y}; }
static inline fvec2 fvec2_mul(const fvec2 v1, const fvec2 v2) { return (fvec2){v1.x * v2.x, v1.y * v2.y}; }
static inline fvec2 fvec2_div(const fvec2 v1, const fvec2 v2) { return (fvec2){v1.x / v2.x, v1.y / v2.y}; }
static inline fvec2 fvec2_scale(const fvec2 v, float s) { return (fvec2){v.x * s, v.y * s}; }
static inline float fvec2_dot(const fvec2 v1, const fvec2 v2) { return v1.x*v2.x + v1.y*v2.y; }
static inline float fvec2_len(const fvec2 v) { return sqrtf(fvec2_dot(v, v)); }
static inline float fvec2_len2(const fvec2 v) { return fvec2_dot(v, v); }
static inline float fvec2_dist(const fvec2 a, const fvec2 b) { return fvec2_len(fvec2_sub(a, b)); }
static inline float fvec2_dist2(const fvec2 a, const fvec2 b) { return fvec2_len2(fvec2_sub(a, b)); }
static inline void fvec2_norm(fvec2* v) { float len = fvec2_len(*v); if (len > 0) { len = 1 / len; v->x *= len; v->y *= len; }}
static inline fvec2 fvec2_normalized(const fvec2 v) { float len = fvec2_len(v); if (len > 0) { len = 1 / len; return FVEC2(v.x*len, v.y*len);} return FVEC2(0.0f, 0.0f);}


static inline fvec3 fvec3_add(const fvec3 v1, const fvec3 v2) { return (fvec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline fvec3 fvec3_sub(const fvec3 v1, const fvec3 v2) { return (fvec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline fvec3 fvec3_mul(const fvec3 v1, const fvec3 v2) { return (fvec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline fvec3 fvec3_div(const fvec3 v1, const fvec3 v2) { return (fvec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline fvec3 fvec3_scale(const fvec3 v, float s) { return (fvec3){v.x * s, v.y * s, v.z * s}; }
static inline float fvec3_dot(const fvec3 v1, const fvec3 v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
static inline float fvec3_len(const fvec3 v) { return sqrtf(fvec3_dot(v, v)); }
static inline float fvec3_len2(const fvec3 v) { return fvec3_dot(v, v); }
static inline float fvec3_dist(const fvec3 a, const fvec3 b) { return fvec3_len(fvec3_sub(a, b)); }
static inline float fvec3_dist2(const fvec3 a, const fvec3 b) { return fvec3_len2(fvec3_sub(a, b)); }
static inline void fvec3_norm(fvec3* v) { float len = fvec3_len(*v); if (len > 0) { len = 1 / len; v->x *= len; v->y *= len; v->z *= len;}}
static inline fvec3 fvec3_normalized(const fvec3 v) { float len = fvec3_len(v); if (len > 0) { len = 1 / len; return FVEC3(v.x*len, v.y*len, v.z*len);} return FVEC3(0.0f, 0.0f, 0.0f);}
static inline fvec3 fvec3_cross(const fvec3 a, const fvec3 b) {
    float cx = a.y*b.z - a.z*b.y;
    float cy = a.z*b.x - a.x*b.z;
    float cz = a.x*b.y - a.y*b.x;
    return FVEC3(cx,cy,cz);
}

/* Debug */
#define IVEC2_PRINT(v) do { printf("[IVEC2: %d, %d]\n", (v).x, (v).y); } while (0)
#define IVEC3_PRINT(v) do { printf("[IVEC3: %d, %d, %d]\n", (v).x, (v).y, (v).z); } while (0)
#define FVEC2_PRINT(v) do { printf("[FVEC2: %f, %f]\n", (v).x, (v).y); } while (0)
#define FVEC3_PRINT(v) do { printf("[FVEC3: %f, %f, %f]\n", (v).x, (v).y, (v).z); } while (0)

#endif /* ETER_MATH_H_ [etesai 2025]*/
