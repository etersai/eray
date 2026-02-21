#include <stdio.h>      /* printf, NULL */
#include <stdint.h>
int main(void)
{
    size_t num = 4;
    for (uint64_t i = 0; i < 64; i++) {
        if (num <= 1ull<<i) {
            num = 1ull<<i;  
            break;
        }
    }
    printf("%zu", num);
}
