#ifndef ERAY_PLATFORM_H_
#define ERAY_PLATFORM_H_

#include "STB/stb_image.h"
#if 0
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

char* map_file_into_memory(const char* filename, size_t* out_file_size);
void unmap_file_from_memory(char* file, size_t size);
char* os_get_cwd(char* buf, size_t size);


#endif /* ERAY_PLATFORM_H_ */
