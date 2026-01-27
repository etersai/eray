#ifndef ERAY_SHADER_H_
#define ERAY_SHADER_H_
#include <glad/glad.h>

typedef GLint uniform;

typedef struct {
    GLuint id;
} ShaderProgram;

ShaderProgram shader_create_from_memory(const char* vertex_shader_src, const char* fragment_shader_src);
void shader_bind(ShaderProgram program);
void shader_delete(ShaderProgram program);
uniform shader_get_uniform_location(ShaderProgram program, const char* name);
void shader_set_mat4_by_name(ShaderProgram program, const char* name, const float* matrix);
void shader_set_mat4(ShaderProgram program, uniform unifrom, const float* matrix);

#endif /* ERAY_SHADER_H_ */
