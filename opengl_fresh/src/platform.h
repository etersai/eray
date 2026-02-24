#ifndef ERAY_PLATFORM_H_
#define ERAY_PLATFORM_H_

#include "common/types.h"
#include "common/str.h"

#include "STB/stb_image.h"
#if 0
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

// forward declare.
typedef struct Arenka Arenka;

#define OS_MAX_PATH (4096)

typedef enum {
    OS_DIR_ERROR_NONE = 0,
    OS_DIR_ERROR_BAD_DIRECTORY,
    OS_DIR_ERROR_NAME_TOO_LONG,
    OS_DIR_ERROR_PERMISSION_DENIED,
    OS_DIR_ERROR_LIMIT_REACHED,
    OS_DIR_ERROR_NOT_A_DIRECTORY,
    OS_DIR_ERROR_MEM_FAIL
} OS_DIR_ERROR;

typedef enum {
    OS_FILE_UNKNOWN = 0,
    OS_FILE_REGULAR, 
    OS_FILE_SYMLINK,
    OS_FILE_DIRECTORY,
    OS_FILE_SOMETHING_ELSE_TODO_BASICALLY,
} os_file_type; 

typedef struct {
    string8      name; 
    os_file_type type;  
} OsDirectoryEntry;

typedef struct {
    OsDirectoryEntry* entries;
    u64               count;
} OsDirectoryContents;

typedef struct {
    char* data;
    u64 size;
} MappedFile;

MappedFile os_map_file(const char* path);
void os_unmap_file(MappedFile* file);
OsDirectoryContents os_get_directory_contents(Arenka* arena, string8 dirpath);
char* os_get_cwd(char* buf, size_t size);

#endif /* ERAY_PLATFORM_H_ */
