#include "obj_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "filesystem.h" 
#include "common/elog.h"
#include "common/da.h"
#include "common/types.h"
#include "common/elog.h"

internal u64 align_to_the_next_power_of_two(u64 num)
{
    for (i32 i=0;i<64;i++) {
        if (num<=1ull<<i) {
            return 1ull<<i;  
        }
    }
    return num;
}

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

// internal void dalog(DaF32 da)
// {
//     elog_p(da.data);
//     elog_zu(da.count);
//     elog_zu(da.capacity);
//     elog_zu(da_size_in_bytes(&da));
// }
//
// internal void dalogalien(DaObjFaceIdx da)
// {
//     elog_p(da.data);
//     elog_zu(da.count);
//     elog_zu(da.capacity);
//     elog_zu(da_size_in_bytes(&da));
// }


ObjInMemory load_obj_final(char* file, u64 size)
{
    assert(file);
    obj_in_memory_v2 obj = {0};

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
            da_append(&obj.vertices, f1);
            da_append(&obj.vertices, f2);
            da_append(&obj.vertices, f3);

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
            da_append(&obj.texcoords, f1);
            da_append(&obj.texcoords, f2);

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
            da_append(&obj.normals, f1);
            da_append(&obj.normals, f2);
            da_append(&obj.normals, f3);

            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }
        else if (*ptr == 'f') { // face
            while (isdigit(*ptr) == 0) { // stop at first digit (indices can't be negative)
                ptr++;
            } 

            u32 temp[3] = {0};
            u32 count = 0;
            char* endPtr = ptr;
            while (*endPtr != '\n') {

                if (*endPtr == '/') {
                    count++;
                    endPtr++;
                    continue;
                }

                u32 i = strtoul(endPtr, &endPtr, 10); 
                temp[count] = i-1; // opengl starts indices at 0, while obj 1.
                if (*endPtr == ' ' || *endPtr == '\n') {
                    obj_face_index ofi;
                    ofi.v  = temp[0];
                    ofi.vt = temp[1];
                    ofi.vn = temp[2];
                    da_append(&obj.faces, ofi);
                    count = 0;
                    if (*endPtr == ' ') endPtr++;
                    continue;
                }
            }
            
            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }

        ptr++;
    }

    // 2904 structs each 3 v,vt,vn.
    // 2904*8 = 23232 :D vertex array. 
    // 2904 = 2904 index array.
    // 1 opengl vertex = 3 floats pos, 2 floats texture, 3 floats normals = 8 per vertex. 

    // NOW FLATTEN!  
    Daf32 gl_vertex_data = {0};
    Dau32 gl_index_data = {0};
    enum { FLOATS_PER_VERTEX = 8 }; // shaky shaky.
    da_init(&gl_vertex_data, align_to_the_next_power_of_two(obj.faces.count*FLOATS_PER_VERTEX));
    da_init(&gl_index_data, align_to_the_next_power_of_two(obj.faces.count));


    for (u64 i = 0; i < obj.faces.count; i++) {
        
        da_append(&gl_vertex_data, obj.vertices.data[obj.faces.data[i].v*3]);
        da_append(&gl_vertex_data, obj.vertices.data[obj.faces.data[i].v*3+1]);
        da_append(&gl_vertex_data, obj.vertices.data[obj.faces.data[i].v*3+2]);

        da_append(&gl_vertex_data, obj.texcoords.data[obj.faces.data[i].vt*2]);
        da_append(&gl_vertex_data, obj.texcoords.data[obj.faces.data[i].vt*2+1]);

        da_append(&gl_vertex_data, obj.normals.data[obj.faces.data[i].vn*3]);
        da_append(&gl_vertex_data, obj.normals.data[obj.faces.data[i].vn*3+1]);
        da_append(&gl_vertex_data, obj.normals.data[obj.faces.data[i].vn*3+2]);


        da_append(&gl_index_data, i);
    }

    da_free(&obj.faces);
    da_free(&obj.vertices);
    da_free(&obj.texcoords);
    da_free(&obj.normals);

    return (ObjInMemory){gl_vertex_data, gl_index_data};
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

            u32 temp[3] = {0};
            u32 count = 0;
            char* endPtr = ptr;
            while (*endPtr != '\n') {

                if (*endPtr == '/') {
                    count++;
                    endPtr++;
                    continue;
                }

                u32 i = strtoul(endPtr, &endPtr, 10); 
                temp[count] = i-1; // opengl starts indices at 0, while obj 1.
                if (*endPtr == ' ' || *endPtr == '\n') {
                    obj_face_index ofi;
                    ofi.v  = temp[0];
                    ofi.vt = temp[1];
                    ofi.vn = temp[2];
                    da_append(&obj->faces, ofi);
                    count = 0;
                    if (*endPtr == ' ') endPtr++;
                    continue;
                }
            }
            
            assert(*endPtr=='\n'); // this lives on this premise xd
            size_t dst = endPtr-ptr;
            ptr+=dst+1; // +1 to skip to next line
                        
            continue;
        }

        ptr++;
    }
        
    // 2904 structs each 3 v,vt,vn.
    // 2904*8 = 23232 :D vertex array. 
    // 2904 = 2904 index array.
    // 1 opengl vertex = 3 floats pos, 2 floats texture, 3 floats normals = 8 per vertex. 

    // NOW FLATTEN!  
    Daf32 gl_vertex_data = {0};
    Dau32 gl_index_data = {0};
    enum { FLOATS_PER_VERTEX = 8 }; // shaky shaky.
    da_init(&gl_vertex_data, align_to_the_next_power_of_two(obj->faces.count*FLOATS_PER_VERTEX));
    da_init(&gl_index_data, align_to_the_next_power_of_two(obj->faces.count));


 //   u64 c = 0;
    for (u64 i = 0; i < obj->faces.count; i++) {
       // elog_u(obj->faces.data[i].v);
       // elog_u(obj->faces.data[i].vt);
       // elog_u(obj->faces.data[i].vn);
        
        // v
        // gl_vertex_data.data[c] = obj->vertices.data[obj->faces.data[i].v*3];
        // gl_vertex_data.data[c+1] = obj->vertices.data[obj->faces.data[i].v*3+1];
        // gl_vertex_data.data[c+2] = obj->vertices.data[obj->faces.data[i].v*3+2];
        //
        // // vt
        // gl_vertex_data.data[c+3] = obj->texcoords.data[obj->faces.data[i].vt*2]; 
        // gl_vertex_data.data[c+4] = obj->texcoords.data[obj->faces.data[i].vt*2+1]; 
        //
        // // vn 
        // gl_vertex_data.data[c+5] = obj->normals.data[obj->faces.data[i].vn*3]; 
        // gl_vertex_data.data[c+6] = obj->normals.data[obj->faces.data[i].vn*3+1]; 
        // gl_vertex_data.data[c+7] = obj->normals.data[obj->faces.data[i].vn*3+2]; 
        
        da_append(&gl_vertex_data, obj->vertices.data[obj->faces.data[i].v*3]);
        da_append(&gl_vertex_data, obj->vertices.data[obj->faces.data[i].v*3+1]);
        da_append(&gl_vertex_data, obj->vertices.data[obj->faces.data[i].v*3+2]);

        da_append(&gl_vertex_data, obj->texcoords.data[obj->faces.data[i].vt*2]);
        da_append(&gl_vertex_data, obj->texcoords.data[obj->faces.data[i].vt*2+1]);

        da_append(&gl_vertex_data, obj->normals.data[obj->faces.data[i].vn*3]);
        da_append(&gl_vertex_data, obj->normals.data[obj->faces.data[i].vn*3+1]);
        da_append(&gl_vertex_data, obj->normals.data[obj->faces.data[i].vn*3+2]);

        //gl_index_data.data[i] = i;
        da_append(&gl_index_data, i);

        // elog_f(obj->vertices.data[obj->faces.data[i].v*3]); 
        // elog_f(obj->vertices.data[obj->faces.data[i].v*3+1]); 
        // elog_f(obj->vertices.data[obj->faces.data[i].v*3+2]); 
       // c+=FLOATS_PER_VERTEX;
    }

    elog_zu(gl_vertex_data.capacity);
    elog_zu(gl_vertex_data.count);

    elog_zu(gl_index_data.capacity);
    elog_zu(gl_index_data.count);


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
