#ifndef ERAY_SHADER_H_
#define ERAY_SHADER_H_
#include <glad/glad.h>

typedef GLint uniform;

typedef struct {
    GLuint id;
} ShaderProgram;

ShaderProgram shader_prog_create_from_memory(const char* vertex_shader_src, const char* fragment_shader_src);
void shader_prog_use(ShaderProgram program);
void shader_prog_delete(ShaderProgram program);
uniform shader_get_uniform_location(ShaderProgram program, const char* name);
void shader_set_mat4_by_name(ShaderProgram program, const char* name, const float* matrix);
void shader_set_vec3(ShaderProgram program, uniform uniform, const float* vec3);
void shader_set_vec4(ShaderProgram program, uniform uniform, const float* vec4);
void shader_set_3fv(ShaderProgram program, uniform uniform, int count, const float* fv);
void shader_set_mat4(ShaderProgram program, uniform unifrom, const float* matrix);
void shader_set_float(ShaderProgram program, uniform uniform, const float f);

#endif /* ERAY_SHADER_H_ */
