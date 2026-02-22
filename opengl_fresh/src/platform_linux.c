#include "platform.h"
#include "common/elog.h"
#include "common/str.h"
#include "common/types.h"

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

// this should probaby go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h" 

#if 0 /* made my own XD */
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

#define str8_to_cstr_buffer(buffer, str8) do { \
    assert((str8).size < sizeof((buffer))); \
    memcpy((buffer), (str8).bytes, (str8).size); \
    (buffer)[(str8).size] = '\0'; \
} while (0)

typedef enum {
    OS_DIR_ERROR_NONE = 0,
    OS_DIR_ERROR_BAD_DIRECTORY,
    OS_DIR_ERROR_NAME_TOO_LONG,
    OS_DIR_ERROR_PERMISSION_DENIED,
    OS_DIR_ERROR_LIMIT_REACHED,
    OS_DIR_ERROR_NOT_A_DIRECTORY,
    OS_DIR_ERROR_MEM_FAIL
} OS_DIR_ERROR;

// top of win32_platform.c
// #define _CRT_SECURE_NO_WARNINGS
// MSVC thing => scanf - scanf_s WINDOWS THINGS XD

void os_list_directory(string8 path)
{
    char path_as_cstr[OS_MAX_PATH];
    str8_to_cstr_buffer(path_as_cstr, path); // assert should be handled diffrently.

    DIR* dir = opendir(path_as_cstr);
    if (dir == NULL) {
        elog_abort("dir null");
    }
    
    errno = 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0) continue;
        if (strcmp(ent->d_name, "..") == 0) continue;

    }
    if (errno != 0) {
        elog_abort("Problem reading dir");
    }

    closedir(dir); // returns -1 on error, but that shoudl not happen?
}

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



