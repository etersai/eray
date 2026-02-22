#ifndef ERAY_PLATFORM_H_
#define ERAY_PLATFORM_H_

#include "common/types.h"
#include "common/str.h"

#include "STB/stb_image.h"
#if 0
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

#define OS_MAX_PATH (4096)

typedef struct {
    char* data;
    u64 size;
} MappedFile;

MappedFile os_map_file(const char* path);
void os_unmap_file(MappedFile* file);
void os_list_directory(string8 path);
char* os_get_cwd(char* buf, size_t size);

#endif /* ERAY_PLATFORM_H_ */
