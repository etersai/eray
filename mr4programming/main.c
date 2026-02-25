#include "base.h"
#include <stdio.h>
#define fast_log(msg) printf("%s\n", msg);
int main(int argc, char* argv[])
{ 

    int x = 5;
    int* p = &x;
    printf("%llu", IntFromPtr(p));

    printf("[COMPILER]\n");
    printf("    gcc = %d\n", COMPILER_GCC);
    printf("    cl = %d\n", COMPILER_CL);
    printf("    clang = %d\n", COMPILER_CLANG);
    printf("    tcc = %d\n", COMPILER_TCC);
    printf("[OPERATING SYSTEM]\n");
    printf("    windows = %d\n", OS_WINDOWS);
    printf("    linux = %d\n", OS_LINUX);
    return 0;
}
