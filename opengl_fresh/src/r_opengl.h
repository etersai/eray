#ifndef GL_STUFF_H_
#define GL_STUFF_H_

#include <glad/glad.h>
#include <stddef.h>
#include "common/types.h" // color

typedef struct {
    GLuint VBO;
    size_t size_in_bytes;
    void* mem_start; 
} GpuPMappedBuffer; 

typedef struct {
    GLuint VAO; 
    GLuint VBO; 
    GLsizei vertex_count;
} GpuMeshSimple;

typedef struct {
    GLuint VAO; 
    GLuint VBO; 
    GLuint EBO;
    GLsizei index_count;
} GpuMeshIndexed;

typedef struct {
    GLuint id; 
    int width;
    int height;
    int num_color_channels;
} Texture;

typedef struct {
    GLuint id;
} TextureCubemap;

typedef enum {
    VERTEX_LAYOUT_POS = 0,
    VERTEX_LAYOUT_POS_TEX,
    VERTEX_LAYOUT_POS_COLOR,
    VERTEX_LAYOUT_POS_NORM_TEX,
    VERTEX_LAYOUT_CUBE_POS, // kinda meh
} VertexLayout;

#define gl_texture_valid(texture) ((texture).id != 0)
#define gl_buffer_valid(buffer) ((buffer).VBO != 0)

void gl_enable_depth_test(void);
void gl_set_clear_color(Colorek color);
void gl_set_viewport(int x, int y, int width, int height);

GpuPMappedBuffer gpu_p_mapped_buffer_create(size_t size_in_bytes);
void gpu_p_mapped_buffer_destroy(GpuPMappedBuffer* buffer);
void* gpu_p_mapped_buffer_write(GpuPMappedBuffer* buffer, size_t offset_in_bytes, size_t size_in_bytes, const void* source);

// kinda sus \/.
GLuint gpu_vao_create_for_text_buffer(GLuint VBO);
// kinda sus ^. 

GpuMeshIndexed gpu_load_mesh_indexed(const float* v, const unsigned int* i, size_t v_size, size_t i_size, VertexLayout layout);
GpuMeshSimple gpu_load_mesh_simple(const float* v, size_t v_size);
void gpu_delete_mesh_indexed(GpuMeshIndexed mesh_indexed);
void gpu_delete_mesh_simple(GpuMeshSimple mesh_simple);

Texture texture_create_from_memory(unsigned char* data, int width, int height, int color_channels);
TextureCubemap texture_cubemap_create_from_paths(const char** paths);

#endif /* GL_STUFF_H_ */
