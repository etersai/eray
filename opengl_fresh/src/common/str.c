#include "types.h"
#include "string.h"
#include "../lamath.h"

typedef struct {
    u8* str;    
    u64 size;
} string8;

void str8(u8* str, u64 size)
{
   unused(str); 
   unused(size); 
}

#define str8_lit(S) str8((u8*)(S), sizeof(S) - 1)

int main(void)
{
    str8_lit("huju jebany");
    

    return 0;
}
