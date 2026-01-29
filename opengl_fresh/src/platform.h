#ifndef ERAY_PLATFORM_H_
#define ERAY_PLATFORM_H_

#include "STB/stb_image.h"
#include "tinyobj_loader_c/tinyobj_loader_c.h"

char* map_file_into_memory(const char* filename, size_t* out_file_size);
int read_obj(const char* filename);
char* eray_get_cwd(char* buf, size_t size);


#endif /* ERAY_PLATFORM_H_ */
