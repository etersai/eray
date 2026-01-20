#include "lamath.h"
#include "linmath.h"


void linma_print(mat4x4 m)
{
    printf("[%f, %f, %f, %f]\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("[%f, %f, %f, %f]\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("[%f, %f, %f, %f]\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("[%f, %f, %f, %f]\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}


int main(void)
{
    matf4x4 rot;
    matf4x4_rot_x(&rot, deg_to_rad(90.0f));
    matf4x4_print(&rot);

    mat4x4 other;
    mat4x4 result;
    mat4x4_identity(other);
    mat4x4_rotate_X(result, other, deg_to_rad(90.0f));
    linma_print(result);
    return 0;
}
