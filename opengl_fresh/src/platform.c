#include "platform.h"

#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h" 

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"

char* map_file_into_memory(const char* filename, size_t* out_file_size)
{
    struct stat sb;
    char* mapped_file;
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    if (fstat(fd, &sb) == -1) {
        close(fd);
        perror("fstat");
        return NULL;
    }

    if (!S_ISREG(sb.st_mode)) { // if not a regular file.
        fprintf(stderr, "%s is not a file\n", filename);
        close(fd);
        return NULL;
    }

    mapped_file = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); 
    if (mapped_file == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    close(fd);

    (*out_file_size) = sb.st_size;
    return mapped_file;
}

char* eray_get_cwd(char* buf, size_t size)
{ // get windows todo. something like _getcwd but chceck!
    return getcwd(buf, size);
}

int read_obj(const char* filename)
{
(void)filename;
return 0;
}
