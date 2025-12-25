/* Public Domain.
** #define ETER_VEC_STRUCT or ETER_VEC_ARRAY  
**  before including this header. 
*/
#ifndef ETER_MATH_H_
#define ETER_MATH_H_

// Basic operations.
#define ETER_SQUARE(x) ((x)*(x))

/************************/
/* Struct Based Vectors */
/************************/
#if (defined(ETER_VEC_STRUCT) && !defined(ETER_VEC_ARRAY))

typedef struct {
    int x;
    int y;
} ivec2;

typedef struct {
    float x;
    float y;
} fvec2;

typedef struct {
    float x;
    float y;
    float z;
} fvec3;


#define IVEC2(x,y) ((ivec2){(x),(y)})
#define IVEC2_ADD(v1,v2) ((ivec2){((v1).x+(v2).x), ((v1).y+(v2).y)})
#define IVEC2_SUB(v1,v2) ((ivec2){((v1).x-(v2).x), ((v1).y-(v2).y)})
#define IVEC2_MUL(v1,v2) ((ivec2){((v1).x*(v2).x), ((v1).y*(v2).y)})
#define IVEC2_DIV(v1,v2) ((ivec2){((v1).x/(v2).x), ((v1).y/(v2).y)})
#define IVEC2_SCALE(v, s) ((ivec2){(v).x*(s), (v).y*(s)})

#define IVEC2_PRINT(v) do {                    \
    printf("[IVEC2: %d, %d]\n", (v).x, (v).y); \
} while (0)


#define FVEC2(x,y) ((fvec2){(x),(y)})
#define FVEC2_ADD(v1,v2) ((fvec2){((v1).x+(v2).x), ((v1).y+(v2).y)})
#define FVEC2_SUB(v1,v2) ((fvec2){((v1).x-(v2).x), ((v1).y-(v2).y)})
#define FVEC2_MUL(v1,v2) ((fvec2){((v1).x*(v2).x), ((v1).y*(v2).y)})
#define FVEC2_DIV(v1,v2) ((fvec2){((v1).x/(v2).x), ((v1).y/(v2).y)})
#define FVEC2_SCALE(v, s) ((fvec2){(v).x*(s), (v).y*(s)})

#define FVEC2_PRINT(v) do {                    \
    printf("[FVEC2: %f, %f]\n", (v).x, (v).y); \
} while (0)


#define FVEC3(x,y,z) ((fvec3){(x), (y), (z)})
#define FVEC3_ADD(v1,v2) ((fvec3){((v1).x+(v2).x), ((v1).y+(v2).y), ((v1).z+(v2).z)})
#define FVEC3_SUB(v1,v2) ((fvec3){((v1).x-(v2).x), ((v1).y-(v2).y), ((v1).z-(v2).z)})
#define FVEC3_MUL(v1,v2) ((fvec3){((v1).x*(v2).x), ((v1).y*(v2).y), ((v1).z*(v2).z)})
#define FVEC3_DIV(v1,v2) ((fvec3){((v1).x/(v2).x), ((v1).y/(v2).y), ((v1).z/(v2).z)})
#define FVEC3_SCALE(v, s) ((fvec3){(v).x*(s), (v).y*(s), (v).z*(s)})

#define FVEC3_PRINT(v) do {                    \
    printf("[FVEC3: %f, %f, %f]\n", (v).x, (v).y, (v).z); \
} while (0)


/***********************/
/* Array based vectors */
/***********************/
#elif defined(ETER_VEC_ARRAY) && !defined(ETER_VEC_STRUCT)

typedef int ivec2[2];                         
typedef float fvec2[2];
typedef float fvec3[3];

#define IVEC2_ADD(v1, v2, result) do { \
    (result)[0] = (v1)[0] + (v2)[0];   \
    (result)[1] = (v1)[1] + (v2)[1];   \
} while (0)

#define IVEC2_SUB(v1, v2, result) do { \
    (result)[0] = (v1)[0] - (v2)[0];   \
    (result)[1] = (v1)[1] - (v2)[1];   \
} while (0)

#define IVEC2_MUL(v1, v2, result) do { \
    (result)[0] = (v1)[0] * (v2)[0];   \
    (result)[1] = (v1)[1] * (v2)[1];   \
} while (0)

#define IVEC2_DIV(v1, v2, result) do { \
    (result)[0] = (v1)[0] / (v2)[0];   \
    (result)[1] = (v1)[1] / (v2)[1];   \
} while (0)

#define IVEC2_SCALE(v, s, result) do { \
    (result)[0] = ((v)[0] * (s)); \
    (result)[1] = ((v)[1] * (s)); \
} while (0)

#define IVEC2_PRINT(v) do { \
    printf("[IVEC2: %d, %d]\n", (v)[0], (v)[1]); \
} while (0)


#define FVEC2_ADD(v1, v2, result) do { \
    (result)[0] = (v1)[0] + (v2)[0];   \
    (result)[1] = (v1)[1] + (v2)[1];   \
} while (0)

#define FVEC2_SUB(v1, v2, result) do { \
    (result)[0] = (v1)[0] - (v2)[0];   \
    (result)[1] = (v1)[1] - (v2)[1];   \
} while (0)

#define FVEC2_MUL(v1, v2, result) do { \
    (result)[0] = (v1)[0] * (v2)[0];   \
    (result)[1] = (v1)[1] * (v2)[1];   \
} while (0)

#define FVEC2_DIV(v1, v2, result) do { \
    (result)[0] = (v1)[0] / (v2)[0];   \
    (result)[1] = (v1)[1] / (v2)[1];   \
} while (0)

#define FVEC2_SCALE(v, s, result) do { \
    (result)[0] = ((v)[0] * (s)); \
    (result)[1] = ((v)[1] * (s)); \
} while (0)

#define FVEC2_PRINT(v) do { \
    printf("[FVEC2: %f, %f]\n", (v)[0], (v)[1]); \
} while (0)


#define FVEC3_ADD(v1, v2, result) do { \
    (result)[0] = (v1)[0] + (v2)[0];   \
    (result)[1] = (v1)[1] + (v2)[1];   \
    (result)[2] = (v1)[2] + (v2)[2];   \
} while (0)

#define FVEC3_SUB(v1, v2, result) do { \
    (result)[0] = (v1)[0] - (v2)[0];   \
    (result)[1] = (v1)[1] - (v2)[1];   \
    (result)[2] = (v1)[2] - (v2)[2];   \
} while (0)

#define FVEC3_MUL(v1, v2, result) do { \
    (result)[0] = (v1)[0] * (v2)[0];   \
    (result)[1] = (v1)[1] * (v2)[1];   \
    (result)[2] = (v1)[2] * (v2)[2];   \
} while (0)

#define FVEC3_DIV(v1, v2, result) do { \
    (result)[0] = (v1)[0] / (v2)[0];   \
    (result)[1] = (v1)[1] / (v2)[1];   \
    (result)[2] = (v1)[2] / (v2)[2];   \
} while (0)

#define FVEC3_SCALE(v, s, result) do { \
    (result)[0] = ((v)[0] * (s)); \
    (result)[1] = ((v)[1] * (s)); \
    (result)[2] = ((v)[2] * (s)); \
} while (0)

#define FVEC3_PRINT(v) do { \
    printf("[FVEC3: %f, %f, %f]\n", (v)[0], (v)[1], (v)[2]); \
} while (0)



#else
    #error "Define either ETER_VEC_STRUCT or ETER_VEC_ARRAY!"
#endif /* ETER_VEC_STRUCT/ETER_VEC_ARRAY */ 

#endif /* ETER_VEC_H_ [etersai 2025]*/
