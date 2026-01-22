#ifndef ERAY_SHADER_H_
#define ERAY_SHADER_H_
#include <glad/glad.h>

typedef GLint uniform;

typedef struct {
    GLuint id;
} ShaderProgram;

ShaderProgram shader_create_from_memory(const char* vertex_shader_src, const char* fragment_shader_src);
inline void shader_bind(ShaderProgram program);
inline void shader_delete(ShaderProgram program);
void shader_uniform_set_by_name_matrix4x4(ShaderProgram program, const char* name, const float* matrix);
void shader_uniform_set_matrix4x4(ShaderProgram program, uniform unifrom, const float* matrix);

#endif /* ERAY_SHADER_H_ */
