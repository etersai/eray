#include "obj_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/types.h"
#include "platform.h" // for file mapping
#include "common/elog.h"

// Dynamic array.
#define DA_INIT_CAP 1024
#define da_size_in_bytes(da) ((da)->count*sizeof(*(da)->data))
#define da_free(da) do { free((da)->data); (da)->count = 0; (da)->capacity = 0; } while (0)
#define da_append(da, val) do { \
     if ((da)->count >= (da)->capacity) { \
        if ((da)->data == NULL) { \
            (da)->data = malloc(DA_INIT_CAP*sizeof(*(da)->data)); \
            elog_perma_assert((da)->data); \
            (da)->capacity = DA_INIT_CAP; \
        } \
        else \
        { \
            (da)->data = realloc((da)->data, da_size_in_bytes(da)*2); \
            elog_perma_assert((da)->data); \
            (da)->capacity = (da)->capacity*2; \
        } \
    } \
    (da)->data[(da)->count++] = (val); \
} while (0) 

// (da)->data = realloc((da)->data, (da)->capacity*2*sizeof(*(da)->data));
int load_obj_from_path(obj_in_memory* obj, const char* path)
{
    assert(obj);
    assert(path);
    MappedFile file = os_map_file(path);
    if (file.data == NULL) {
        return 1;
    }
    load_obj_to_buffers_not_safe(obj, file.data, file.size);
    os_unmap_file(&file);
    return 0;
}

// float strtof(const char *nptr, char **endptr);
// char szOrbits[] = "686.97 365.24";
// char* pEnd;
// float f1, f2;
// f1 = strtof (szOrbits, &pEnd);
// f2 = strtof (pEnd, NULL);
// printf ("One martian year takes %.2f Earth years.\n", f1/f2);
// return 0;


void dalog(DaF32 da)
{
    elog_p(da.data);
    elog_zu(da.count);
    elog_zu(da.capacity);
    elog_zu(da_size_in_bytes(&da));
}


void load_obj_test(obj_in_memory_v2* obj, char* file, u64 size)
{
    assert(obj);
    assert(file);

    char* ptr = file;
    char* end = file + size;
    while (ptr < end) {
        if (*ptr == 'v' && *(ptr+1) == ' ') { // vertex
            while (isdigit(*ptr) == 0 && *ptr != '-') { // stop at first digit or -
                ptr++;
            }           
            char* endPtr = NULL;

            f32 f1 = (f32)strtof(ptr, &endPtr);
            f32 f2 = (f32)strtof(endPtr, &endPtr);
            f32 f3 = (f32)strtof(endPtr, &endPtr);
            da_append(&obj->vertices, f1);
            da_append(&obj->vertices, f2);
            da_append(&obj->vertices, f3);

            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }
        else if (*ptr == 'v' && *(ptr+1) == 't') { // texcoord
            while (isdigit(*ptr) == 0) { // stop at first digit (texcoords can't be negative)
                ptr++;
            }
            char* endPtr = NULL;

            f32 f1 = (f32)strtof(ptr, &endPtr);
            f32 f2 = (f32)strtof(endPtr, &endPtr);
            da_append(&obj->texcoords, f1);
            da_append(&obj->texcoords, f2);

            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }
        else if (*ptr == 'v' && *(ptr+1) == 'n') { // normal
            while (isdigit(*ptr) == 0 && *ptr != '-') { // stop at first digit or -
                ptr++;
            }
            char* endPtr = NULL;

            f32 f1 = (f32)strtof(ptr, &endPtr);
            f32 f2 = (f32)strtof(endPtr, &endPtr);
            f32 f3 = (f32)strtof(endPtr, &endPtr);
            da_append(&obj->normals, f1);
            da_append(&obj->normals, f2);
            da_append(&obj->normals, f3);

            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }
        else if (*ptr == 'f') { // face
            while (isdigit(*ptr) == 0) { // stop at first digit (indices can't be negative)
                ptr++;
            } 
            char* endPtr = NULL;
            u32 i = strtoul(ptr, &endPtr, 0);
            elog_d(i);
            while (*endPtr != '\n') {
                if (*endPtr == '/') {
                    elog_c(*endPtr);
                    abort();
                }              
            } 

            abort();
        }

        ptr++;
    }
}

void load_obj_to_buffers_not_safe(obj_in_memory* obj, char* file, size_t size)
{
    assert(obj);
    enum { MAX_BUF = 64 };

    char line_buffer[MAX_BUF];
    int line_buffer_count = 0;
    size_t v_count = 0;
    size_t i_count = 0;
    char* ptr = file;
    char* end = file + size;
    while (ptr < end) {

        if (*ptr == '\n') {
            line_buffer[line_buffer_count] = '\0';

            if (line_buffer[0] == 'v') {
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok = strtok(ptr_temp, " "); // strtok modifies buffer 
                while (tok) {
                    float val;
                    int result = sscanf(tok, "%f", &val);
                    if (result == 0) {
                        abort();
                    }
                    obj->vertices[v_count++] = val; 
                    tok = strtok(NULL, " ");
                } 
            }

            else if (line_buffer[0] == 'f') {
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok = strtok(ptr_temp, " "); // strtok puts '\0' at delimiters 
                while (tok) {
                    unsigned int val;
                    int result = sscanf(tok, "%u", &val);
                    if (result == 0) {
                        abort();
                    }                                  //OBJ files use 1-based indexing (indices start at 1),
                    obj->indices[i_count++] = val - 1; //while OpenGL uses 0-based indexing
                    tok = strtok(NULL, " ");
                }
            }
            // go to next line.
            line_buffer_count = 0;
            ptr++;
            continue;
        }

        line_buffer[line_buffer_count] = *ptr;
        line_buffer_count++;
        assert(line_buffer_count < MAX_BUF);
        ptr++;
    }
    obj->v_count = v_count; 
    obj->i_count = i_count; 
}
