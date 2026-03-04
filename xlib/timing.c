#include <time.h>
#include <stdio.h>

int main(void)
{
    time_t t = time(NULL);
    printf("%s\n", ctime(&t));

    struct tm* x = localtime(&t);
    x->tm_hour;
    return 0;
}
