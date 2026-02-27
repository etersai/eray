#ifndef OBJ_LOADER_H_
#define OBJ_LOADER_H_

#include "common/da.h"
#include "common/types.h"
#include <stddef.h>
                            // 1/2 mib
#define OBJ_BUFFERS_BYTE_SIZE (1024*512)

typedef struct {
    float vertices[OBJ_BUFFERS_BYTE_SIZE / sizeof(float)];
    unsigned int indices[OBJ_BUFFERS_BYTE_SIZE / sizeof(unsigned int)];
    size_t v_count;
    size_t i_count;
} obj_in_memory; 

typedef struct {
    u32 v;
    u32 vt;
    u32 vn; 
} obj_face_index;


// expand dynamic arrays.
DA_TYPE(f32)
DA_TYPE(u32)
DA_TYPE(obj_face_index)

typedef struct {
    Daf32 vertices;
    Daf32 texcoords;
    Daf32 normals;
    Daobj_face_index faces;
} obj_in_memory_v2;

typedef struct {
    Daf32 gl_vertex_data;
    Dau32 gl_index_data;
} ObjInMemory;

int load_obj_from_path(obj_in_memory* obj, const char* path);
void load_obj_to_buffers_not_safe(obj_in_memory* obj, char* file, size_t size);
void load_obj_test(obj_in_memory_v2* obj, char* file, u64 size);
ObjInMemory load_obj_final(char* file, u64 size);

#endif /* OBJ_LOADER_H_ */

///////////
// REMINDER
// struct {
//     float x;
//     float y;
//     float w;
//     float h;
// } uv = {
//     .x = (float)character.x / font->texture.width,
//     .y = (float)character.y / font->texture.height,
//     .w = (float)character.width / font->texture.width,
//     .h = (float)character.height / font->texture.height,
// };
//

