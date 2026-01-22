#ifndef ERAY_SHADER_H_
#define ERAY_SHADER_H_
#include <glad/glad.h>

typedef struct {
    GLuint id;
} ShaderProgram;

ShaderProgram shader_create_from_memory(const char* vertex_shader_src, const char* fragment_shader_src);
inline void shader_bind(ShaderProgram program);
inline void shader_delete(ShaderProgram program);

#endif /* ERAY_SHADER_H_ */
