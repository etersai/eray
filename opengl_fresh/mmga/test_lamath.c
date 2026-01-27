#include "lamath.h"
#include "linmath.h"

int main(void)
{
    matf4x4 m = matf4x4_I_give();

    m.col1 = (vecf4){1.0f, 2.0f, -1.0f, 2.0f};
    m.col2 = (vecf4){0.0f, 5.0f,  2.0f, 1.0f};
    m.col3 = (vecf4){4.0f, 0.0f,  3.0f, -2.0f};
    m.col4 = (vecf4){-6.0f, 3.0f, 5.0f, 3.0f};

    matf4x4_print(&m);

    printf("%f\n", matf4x4_det(&m));


    return 0;
}
