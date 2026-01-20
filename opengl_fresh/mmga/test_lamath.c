#include "lamath.h"

int main(void)
{
    vecf4 pos = {1.0f, 1.0f, 1.0f, 1.0f};

    matf4x4 scale;
    matf4x4_scale_set_uniform(&scale, 2.0f, 2.0f, 2.0f);
    
    matf4x4 translate;
    matf4x4_I(&translate);
    translate.col4.x = 2.0f;
    translate.col4.y = 2.0f;
    translate.col4.z = 2.0f;

    matf4x4 result;
    matf4x4_mul(&result, &translate, &scale);

    matf4x4_print(&result);


    return 0;
}
