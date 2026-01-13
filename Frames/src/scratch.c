#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

typedef struct {
    int id;    
    int data;
} Thing; 

Thing        items[256];
size_t       capacity = 256;
size_t       count;

int main(void)
{  
    
    for (int i = 1; i >= 0; i--) {
        printf("[%d]\n", i);
    }

    // // init.
    // count = 0;
    //
    // items[0].data = 10;
    // items[0].id   = 0;
    // count++; // 1
    //
    // items[1].data = 20;
    // items[1].id   = 1;
    // count++; // 2
    //
    //
    // items[2].data = 30;
    // items[2].id   = 2;
    // count++; // 3
    //
    // for (int i = 0; i < count; i++) {
    //
    //     printf("%d, %d\n", items[i].id, items[i].data);
    //
    // }
    //
    // // search for id.
    // int id_to_find = 1; // middle man
    // int found_index = -1;
    // for (int i = 0; i < count; i++) {
    //     if (items[i].id == id_to_find) {
    //         found_index = i;
    //         break;
    //     }
    // }
    // if (found_index < 0) {
    //     abort(); // item with that id not on the lsit.
    // }
    //
    // // found_index == 1;
    // Thing temp;
    // temp.id = items[found_index].id;
    // temp.data = items[found_index].data;
    //
    // for (int i = found_index+1; i < count; i++) {
    //     items[i-1].data = items[i].data;
    //     items[i-1].id = items[i].id;
    // }
    //
    // items[count-1].data = temp.data;
    // items[count-1].id   = temp.id;
    //
    //
    // for (int i = 0; i < count; i++) {
    //
    //     printf("%d, %d\n", items[i].id, items[i].data);
    //
    // }
    //
    //
    //
    // // search for id.
    // id_to_find = 0; // middle man
    // found_index = -1;
    // for (int i = 0; i < count; i++) {
    //     if (items[i].id == id_to_find) {
    //         found_index = i;
    //         break;
    //     }
    // }
    // if (found_index < 0) {
    //     abort(); // item with that id not on the lsit.
    // }
    //
    // // found_index == 1;
    // Thing temp2;
    // temp2.id = items[found_index].id;
    // temp2.data = items[found_index].data;
    //
    // for (int i = found_index+1; i < count; i++) {
    //     items[i-1].data = items[i].data;
    //     items[i-1].id = items[i].id;
    // }
    //
    // items[count-1].data = temp2.data;
    // items[count-1].id   = temp2.id;
    //
    //
    // for (int i = 0; i < count; i++) {
    //
    //     printf("%d, %d\n", items[i].id, items[i].data);
    //
    // }
    //
    // for (int i = 2-1; i >= 0; i--) {
    //     printf("[%d]\n", i);
    // }
    //

    return 0;
}
