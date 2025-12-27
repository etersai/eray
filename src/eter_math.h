// Public Domain
#ifndef ETER_MATH_H_
#define ETER_MATH_H_

/***************************/
/* Basic macros/operations */
/***************************/
#define ETER_MAX(a,b) ((a)>(b)?(a):(b))
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

static inline ivec3 ivec3_add(ivec3 v1, ivec3 v2) { return (ivec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline ivec3 ivec3_sub(ivec3 v1, ivec3 v2) { return (ivec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline ivec3 ivec3_mul(ivec3 v1, ivec3 v2) { return (ivec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline ivec3 ivec3_div(ivec3 v1, ivec3 v2) { return (ivec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline ivec3 ivec3_scale(ivec3 v, int s) { return (ivec3){v.x * s, v.y * s, v.z * s}; }

static inline fvec2 fvec2_add(fvec2 v1, fvec2 v2) { return (fvec2){v1.x + v2.x, v1.y + v2.y}; }
static inline fvec2 fvec2_sub(fvec2 v1, fvec2 v2) { return (fvec2){v1.x - v2.x, v1.y - v2.y}; }
static inline fvec2 fvec2_mul(fvec2 v1, fvec2 v2) { return (fvec2){v1.x * v2.x, v1.y * v2.y}; }
static inline fvec2 fvec2_div(fvec2 v1, fvec2 v2) { return (fvec2){v1.x / v2.x, v1.y / v2.y}; }
static inline fvec2 fvec2_scale(fvec2 v, float s) { return (fvec2){v.x * s, v.y * s}; }

static inline fvec3 fvec3_add(fvec3 v1, fvec3 v2) { return (fvec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
static inline fvec3 fvec3_sub(fvec3 v1, fvec3 v2) { return (fvec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
static inline fvec3 fvec3_mul(fvec3 v1, fvec3 v2) { return (fvec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
static inline fvec3 fvec3_div(fvec3 v1, fvec3 v2) { return (fvec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }
static inline fvec3 fvec3_scale(fvec3 v, float s) { return (fvec3){v.x * s, v.y * s, v.z * s}; }

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

#endif /* ETER_MATH_H_ */
