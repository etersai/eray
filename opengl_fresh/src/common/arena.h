#ifndef ERAY_ARENA_H_
#define ERAY_ARENA_H_

#include <assert.h>
#include <sys/mman.h>                      
#include <stddef.h>

#define KIBIBYTE(val) ((val)*1024)
#define MEBIBYTE(val) ((val)*1024*1024)
#define GIBIBYTE(val) ((val)*1024*1024*1024) 

typedef struct {
    unsigned char* addr_start;
    size_t         offset;
    size_t         capacity;
} Arenka;

#define arenka_wipe(arena) do { (arena)->offset = 0; } while (0)

static Arenka arenka_map(size_t size_in_bytes)
{
    Arenka arena = {0};
    unsigned char* memory_region;

    memory_region = mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (memory_region == MAP_FAILED) {
        return arena;
    }

    arena.addr_start = memory_region;
    arena.capacity = size_in_bytes;
    arena.offset = 0;

    return arena;
}

static unsigned char* arenka_get_piece(Arenka* arena, size_t size_in_bytes)
{
    if (arena->offset + size_in_bytes > arena->capacity) { 
        return NULL;
    }
    unsigned char* return_addr = arena->addr_start + arena->offset;
    arena->offset += size_in_bytes;
    return return_addr;
}

static void arenka_unmap(Arenka* arena)
{
    assert(arena);
    if (arena->addr_start != NULL) {
        munmap(arena->addr_start, arena->capacity);
        arena->addr_start = NULL;
        arena->capacity = 0;
        arena->offset = 0;
    }
}

#endif /* ERAY_ARENA_H_ */
