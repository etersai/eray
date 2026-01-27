#include "lamath.h"
#include "linmath.h"

int main(void)
{
    matf3x3 m = matf3x3_I_give();

    m.col1.x = 1.0f;
    m.col1.y = 9.0f;
    m.col1.z = 1.0f;

    m.col2.x = 2.0f;
    m.col2.y = 5.0f;
    m.col2.z = 8.0f;

    m.col3.x = 2.0f;
    m.col3.y = 7.0f;
    m.col3.z = 1.0f;

    matf3x3_print(&m);

    printf("%f\n", matf3x3_det(&m));


    return 0;
}
