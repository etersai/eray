#include <stdio.h>

int main(void) {
    size_t length = 5;
    char string[length];
    string[0] = 'A';
    string[1] = 'B';
    string[2] = 0;        // null character in the middle
    string[3] = 'C';
    string[4] = 'D';

    printf("With printf: %.*s\n", length, string);
    printf("With fwrite: ");
    fwrite(string, sizeof(char), length, stdout);
    printf("\n");

    return 0;
}
