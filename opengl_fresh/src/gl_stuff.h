#ifndef GL_STUFF_H_
#define GL_STUFF_H_

#include <glad/glad.h>
#include <stddef.h>

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

#define texture_valid(texture) ((texture).id != 0)

GpuMeshIndexed gpu_load_mesh_quad(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size);
GpuMeshIndexed gpu_load_mesh_model(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size);
GpuMeshIndexed gpu_load_mesh_anchor(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size);
GpuMeshIndexed gpu_load_mesh_3attr(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size); 
GpuMeshSimple gpu_load_mesh_simple_1attr(const float* vertices, size_t vertices_size);

Texture texture_create_from_memory(unsigned char* data, int width, int height, int color_channels);
TextureCubemap texture_cubemap_create_from_paths(const char** paths);

#endif /* GL_STUFF_H_ */
