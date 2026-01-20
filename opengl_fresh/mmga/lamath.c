#include "lamath.h"

int main(void)
{
    matf4x4 mat;
    matf4x4_I(&mat);    
    matf4x4_print(&mat);

    vecf3 xd = {5.0f, 6.0f, 7.0f};
    vecf3_print(xd);

    return 0;
}
