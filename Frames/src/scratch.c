#include <stdio.h>
#include <stdarg.h>

#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))
#define mu_stack(T, n) struct { int idx; T items[n]; }

#define stack_push(stack, val) do {            \
    if ((stack).idx < arrlen((stack).items)) { \
        (stack).items[(stack).idx++] = (val);  \
    }                                          \
} while(0)

#define stack_pop(stack, val) do {          \
    if ((stack).idx > 0) {                  \
        (stack).idx--;                      \
        (val) = (stack).items[(stack).idx]; \
    }                                       \
} while (0)


typedef struct {
    mu_stack(int, 64) x;
} Elo;

#define LOG_BUFFER_BEAUTIFY_MAX 64

void log_print_beautify(const char* prefix, const char* format, ...)
{
    int idx = 0;
    char buffer[LOG_BUFFER_BEAUTIFY_MAX] = {0};
    while (*prefix) {
        if (idx >= LOG_BUFFER_BEAUTIFY_MAX) {
            break; 
        }
        buffer[idx] = *prefix;
        prefix++; idx++;
    } // TODO: right now it does nothing. but expand.
    
    fprintf(stderr, "[%s]: ", buffer);
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Use vprintf for va_list
    va_end(args);
}

#include <stdbool.h>
const char* stringify_bool(bool var) { return var ? "TRUE" : "FALSE"; }

int main(void)
{

    printf("%s\n", stringify_bool(false));
    
  

    return 0;
}
