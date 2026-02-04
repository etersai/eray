#ifndef OBJ_LOADER_H_
#define OBJ_LOADER_H_

#include <stddef.h>
                            // 1/2 mib
#define OBJ_BUFFERS_BYTE_SIZE (1024*512)

typedef struct {
    float vertices[OBJ_BUFFERS_BYTE_SIZE / sizeof(float)];
    unsigned int indices[OBJ_BUFFERS_BYTE_SIZE / sizeof(unsigned int)];
    size_t v_count;
    size_t i_count;
} obj_in_memory; 

int load_obj_from_path(obj_in_memory* obj, const char* path);
void load_obj_to_buffers_not_safe(obj_in_memory* obj, char* file, size_t size);

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

