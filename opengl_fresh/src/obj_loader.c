#include "obj_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "platform.h" // for file mapping
#include "common/elog.h"

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

void load_obj_test(obj_in_memory_v2* obj, char* file, u64 size)
{
    assert(obj);
    assert(file);
    char* ptr = file;
    char* end = file + size;
    while (ptr < end) {
        
        if (*ptr == 'v' && *(ptr+1) == ' ') { // vertex
            while (isdigit(*ptr) == 0 || *ptr == '-') {
                ptr++;
            }           
            putchar(*ptr);
            fflush(stdout);
        }
        else if (*ptr == 'v' && *(ptr+1) == 't') { // texcoord
            elog_s("vt"); 
        }
        else if (*ptr == 'v' && *(ptr+1) == 'n') { // normal
            elog_s("vn"); 
        }
        else if (*ptr == 'f') { // face
            elog_s("f");
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
