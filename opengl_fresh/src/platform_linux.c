#define _DEFAULT_SOURCE /* for DT_REG, DT_DIR etc. */
#include "platform.h"
#include "common/types.h"
#include "common/arena.h"
#include "common/elog.h"
#include "common/str.h"

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

#if 0 /* building my own XD */
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#endif

#define str8_to_cstr_buffer(buffer, str8) do { \
    assert((str8).size < sizeof((buffer))); \
    memcpy((buffer), (str8).bytes, (str8).size); \
    (buffer)[(str8).size] = '\0'; \
} while (0)

                                                        // RADgametoolz wannabe style comments XD
internal u64 os_count_files_in_driectory(DIR* dir);/*TODO(eter):this should be abstracted for windows/linux somehow*/

OsDirectoryContents os_get_directory_contents(Arenka* arena, string8 dirpath)
{
    char dirpath_as_cstr[OS_MAX_PATH];
    str8_to_cstr_buffer(dirpath_as_cstr, dirpath); // assert should be handled diffrently.

    DIR* dir = opendir(dirpath_as_cstr);
    if (dir == NULL){
        return (OsDirectoryContents){NULL, 0};
    }/*Thanks GOD*/
    
    u64 file_count = os_count_files_in_driectory(dir);
    OsDirectoryEntry* entries = (OsDirectoryEntry*)arenka_get_piece(arena, sizeof(OsDirectoryEntry)*file_count);

    u64 count = 0;
    errno = 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0)  continue;
        if (strcmp(ent->d_name, "..") == 0) continue;
        switch (ent->d_type)
        {
            case DT_LNK:
                entries[count].name = str8_from_cstr(arena, ent->d_name); 
                entries[count].type = OS_FILE_SYMLINK; 
            break;
            case DT_REG:
                entries[count].name = str8_from_cstr(arena, ent->d_name); 
                entries[count].type = OS_FILE_REGULAR; 
            break;
            case DT_DIR:
                entries[count].name = str8_from_cstr(arena, ent->d_name); 
                entries[count].type = OS_FILE_DIRECTORY; 
            break;
            case DT_UNKNOWN:
                entries[count].name = str8_from_cstr(arena, ent->d_name); 
                entries[count].type = OS_FILE_UNKNOWN;  
            break;
            case DT_BLK: /*fallthrough*/
            case DT_CHR:
            case DT_FIFO:
            case DT_SOCK:
            default:
                entries[count].name = str8_from_cstr(arena, ent->d_name); 
                entries[count].type = OS_FILE_SOMETHING_ELSE_TODO_BASICALLY;  
        }
        count++;
    }
    closedir(dir);
    return (OsDirectoryContents){entries, file_count};
}

void os_list_directory(string8 path)
{
    char path_as_cstr[OS_MAX_PATH];
    str8_to_cstr_buffer(path_as_cstr, path); // assert should be handled diffrently.

    DIR* dir = opendir(path_as_cstr);
    if (dir == NULL) {
        elog_fmt("opendir failed: %s", strerror(errno)); // TODO(eter).
        return;
    }

    
    errno = 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0)  continue;
        if (strcmp(ent->d_name, "..") == 0) continue;
        
        switch (ent->d_type) {
/*symlink*/ case DT_LNK: /*fallthrough*/ 
            case DT_REG: 
                elog_s("REGULAR OR SYMLINK:");
                elog_s(ent->d_name); 
            break;

            case DT_DIR:
                elog_s("DIR:");
                elog_s(ent->d_name);
            break;

            case DT_BLK:/*fallthrough*/
            case DT_CHR: 
            case DT_FIFO:
            case DT_SOCK:
            case DT_UNKNOWN:
            default:
                elog_s("SOMETHING ELSE:");
                elog_s(ent->d_name);
        }

    }
    if (errno != 0) {
        elog_fmt("readdir failed: %s", strerror(errno)); // TODO(eter).
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
                                                        // RADgametoolz wannabe style comments XD
internal u64 os_count_files_in_driectory(DIR* dir) /*TODO(eter):this should be abstracted for windows/linux somehow*/
{
    u64 count = 0;
    errno = 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0)  continue;
        if (strcmp(ent->d_name, "..") == 0) continue;
        switch (ent->d_type)
        {
            case DT_LNK: /*fallthrough*/ 
            case DT_REG: 
            case DT_DIR:
            case DT_BLK:
            case DT_CHR: 
            case DT_FIFO:
            case DT_SOCK:
            case DT_UNKNOWN:
            default:
                count++;
        } /*switch*/
    } /*while*/
    rewinddir(dir);
    if (errno != 0) {
        count = 0;
        return count;
    }
    return count;
}
