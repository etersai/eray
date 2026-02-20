#include "platform.h"

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h" 

#if 0 /* made my own XD */
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

MappedFile os_map_file(const char* path)
{
    MappedFile file = {0};
    struct stat sb;
    int fd;

    fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return file;
    }

    if (fstat(fd, &sb) == -1) {
        close(fd);
        perror("fstat");
        return file;
    }

    if (!S_ISREG(sb.st_mode)) { // if not a regular file.
        fprintf(stderr, "%s is not a file\n", path);
        close(fd);
        return file;
    }

    file.data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); 
    if (file.data == MAP_FAILED) {
        perror("mmap"); 
        close(fd);
        file.data = NULL;
        return file;
    }

    close(fd);
    file.size = sb.st_size;
    return file;
}

void os_unmap_file(MappedFile* file)
{ 
    if (file) {
        munmap(file->data, file->size);
    }
}

char* os_get_cwd(char* buf, size_t size)
{ // TODO: windows idef _getcwd() ?
    return getcwd(buf, size);
}

