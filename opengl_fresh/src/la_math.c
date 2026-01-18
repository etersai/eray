#include <stdio.h>
#include <math.h>
typedef struct { float x; float y; float z; } vecf3;
typedef struct { float data[4][4]; } matf4x4;

static inline float vecf3_len(vecf3 vec);
static inline vecf3 vecf3_add(vecf3 a, vecf3 b);
static inline vecf3 vecf3_sub(vecf3 a, vecf3 b);
static inline vecf3 vecf3_scale(float factor, vecf3 vec);
static inline vecf3 vecf3_norm(vecf3 vec);

static inline void matf4x4_identity(matf4x4* matrix);
static inline void matf4x4_test(matf4x4* matrix);
static inline void matf4x4_multiply(const matf4x4* matrix_a, const matf4x4* matrix_b, matf4x4* result);
static inline void matf4x4_translate(matf4x4* matrix, float x, float y, float z);
static inline void matf4x4_translate_set(matf4x4* matrix, float x, float y, float z);
static inline void matf4x4_scale_uniform(matf4x4* matrix, float scale);
static inline void matf4x4_scale(matf4x4* matrix, float sx, float sy, float sz);
static inline void matf4x4_transpose_to_new(const matf4x4* matrix, matf4x4* result);
static inline void matf4x4_transpose_in_place(matf4x4* matrix);
static inline vecf3 transformPoint(const vecf3* point, const matf4x4* matrix);

static void matf4x4_print(const matf4x4* matrix);
static void vecf3_print(vecf3 vec);

// MY RETARDED MATH
static inline float vecf3_len(vecf3 vec) 
{ 
    return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}
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


#define MATRIX4X4_PRINT_FORMAT "[%.2f, %.2f, %.2f, %.2f]\n"
#define VECF3_PRINT_FORMAT "[%.2f, %.2f, %.2f]\n"
static void matf4x4_print(const matf4x4* matrix)
{
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[0][0], matrix->data[0][1], matrix->data[0][2], matrix->data[0][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[1][0], matrix->data[1][1], matrix->data[1][2], matrix->data[1][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[2][0], matrix->data[2][1], matrix->data[2][2], matrix->data[2][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[3][0], matrix->data[3][1], matrix->data[3][2], matrix->data[3][3]);
    fflush(stdout);
}

static void vecf3_print(vecf3 vec)
{
    fprintf(stdout, VECF3_PRINT_FORMAT, vec.x, vec.y, vec.z);
    fflush(stdout);
}

static inline void matf4x4_identity(matf4x4* matrix)
{
    matrix->data[0][0] = 1.0f;
    matrix->data[0][1] = 0.0f;
    matrix->data[0][2] = 0.0f;
    matrix->data[0][3] = 0.0f;
    
    matrix->data[1][0] = 0.0f;
    matrix->data[1][1] = 1.0f;
    matrix->data[1][2] = 0.0f;
    matrix->data[1][3] = 0.0f; 

    matrix->data[2][0] = 0.0f;
    matrix->data[2][1] = 0.0f;
    matrix->data[2][2] = 1.0f;
    matrix->data[2][3] = 0.0f;
    
    matrix->data[3][0] = 0.0f;
    matrix->data[3][1] = 0.0f;
    matrix->data[3][2] = 0.0f;
    matrix->data[3][3] = 1.0f;
}

static inline void matf4x4_test(matf4x4* matrix)
{
    matrix->data[0][0] = 1.0f;
    matrix->data[0][1] = 2.0f;
    matrix->data[0][2] = 3.0f;
    matrix->data[0][3] = 4.0f;
    
    matrix->data[1][0] = 5.0f;
    matrix->data[1][1] = 6.0f;
    matrix->data[1][2] = 7.0f;
    matrix->data[1][3] = 8.0f; 

    matrix->data[2][0] = 9.0f;
    matrix->data[2][1] = 10.0f;
    matrix->data[2][2] = 11.0f;
    matrix->data[2][3] = 12.0f;
    
    matrix->data[3][0] = 13.0f;
    matrix->data[3][1] = 14.0f;
    matrix->data[3][2] = 15.0f;
    matrix->data[3][3] = 16.0f;
}

static inline void matf4x4_multiply(const matf4x4* matrix_a, const matf4x4* matrix_b, matf4x4* result)
{
      
}



static inline void matf4x4_translate(matf4x4* matrix, float x, float y, float z)
{
    matrix->data[3][0] += x; 
    matrix->data[3][1] += y; 
    matrix->data[3][2] += z; 
}

static inline void matf4x4_translate_set(matf4x4* matrix, float x, float y, float z)
{
    matrix->data[3][0] = x; 
    matrix->data[3][1] = y; 
    matrix->data[3][2] = z; 
}

static inline void matf4x4_scale_uniform(matf4x4* matrix, float scale)
{
    matrix->data[0][0] *= scale;
    matrix->data[1][1] *= scale;
    matrix->data[2][2] *= scale;
}

static inline void matf4x4_scale(matf4x4* matrix, float sx, float sy, float sz)
{
    matrix->data[0][0] *= sx;
    matrix->data[1][1] *= sy;
    matrix->data[2][2] *= sz;
}

static inline void matf4x4_transpose_to_new(const matf4x4* matrix, matf4x4* result)
{
    // first row to column
    result->data[0][0] = matrix->data[0][0];
    result->data[1][0] = matrix->data[0][1];
    result->data[2][0] = matrix->data[0][2];
    result->data[3][0] = matrix->data[0][3];
    
    // second row to second column
    result->data[0][1] = matrix->data[1][0];
    result->data[1][1] = matrix->data[1][1];
    result->data[2][1] = matrix->data[1][2];
    result->data[3][1] = matrix->data[1][3];

    // third row to third column
    result->data[0][2] = matrix->data[2][0];
    result->data[1][2] = matrix->data[2][1];
    result->data[2][2] = matrix->data[2][2];
    result->data[3][2] = matrix->data[2][3];

    // fourth row to fourth column
    result->data[0][3] = matrix->data[3][0];
    result->data[1][3] = matrix->data[3][1];
    result->data[2][3] = matrix->data[3][2];
    result->data[3][3] = matrix->data[3][3];
}


static inline void matf4x4_transpose_in_place(matf4x4* matrix)
{
    matf4x4 temp;
    
    // first row to column
    temp.data[0][0] = matrix->data[0][0];
    temp.data[1][0] = matrix->data[0][1];
    temp.data[2][0] = matrix->data[0][2];
    temp.data[3][0] = matrix->data[0][3];
    
    // second row to second column
    temp.data[0][1] = matrix->data[1][0];
    temp.data[1][1] = matrix->data[1][1];
    temp.data[2][1] = matrix->data[1][2];
    temp.data[3][1] = matrix->data[1][3];

    // third row to third column
    temp.data[0][2] = matrix->data[2][0];
    temp.data[1][2] = matrix->data[2][1];
    temp.data[2][2] = matrix->data[2][2];
    temp.data[3][2] = matrix->data[2][3];

    // fourth row to fourth column
    temp.data[0][3] = matrix->data[3][0];
    temp.data[1][3] = matrix->data[3][1];
    temp.data[2][3] = matrix->data[3][2];
    temp.data[3][3] = matrix->data[3][3];

    *matrix = temp;
}

static inline vecf3 transformPoint(const vecf3* point, const matf4x4* matrix)
{
    float x = point->x * matrix->data[0][0] + point->y * matrix->data[1][0] + 
              point->z * matrix->data[2][0] + matrix->data[3][0]; 
    float y = point->x * matrix->data[0][1] + point->y * matrix->data[1][1] + 
              point->z * matrix->data[2][1] + matrix->data[3][1]; 
    float z = point->x * matrix->data[0][2] + point->y * matrix->data[1][2] + 
              point->z * matrix->data[2][2] + matrix->data[3][2]; 
    return (vecf3){x, y, z};
}
