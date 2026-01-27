#include "lamath.h"
#include "linmath.h"

int main(void)
{
    matf4x4 xd = matf4x4_I_give();
    matf4x4_print(&xd);
    return 0;
}
