#include "shader.h"

#include <stddef.h>
#include <assert.h>

static int shader_compile_error(GLuint shader);
static int shader_program_link_error(GLuint program);

inline void shader_delete(ShaderProgram program)
{
    glDeleteProgram(program.id);
}

inline void shader_bind(ShaderProgram program)
{
    glUseProgram(program.id);
}

ShaderProgram shader_create_from_memory(const char* vertex_shader_src, const char* fragment_shader_src)
{
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);
    if (shader_compile_error(vertex_shader)) {
        glDeleteShader(vertex_shader);
        return (ShaderProgram){0};
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);
    if (shader_compile_error(fragment_shader)) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return (ShaderProgram){0};
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);  
    glDeleteShader(fragment_shader);

    if (shader_program_link_error(program)) {
        glDeleteProgram(program);
        return (ShaderProgram){0};
    }

    return (ShaderProgram){.id = program};
}

static int shader_compile_error(GLuint shader)
{
	GLint success;
	char info_log[1024] = {0};

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        assert(0); // TODO: ADD PROPER LOGGING.
		return 1;
	}
	return 0;
}

static int shader_program_link_error(GLuint program)
{
    GLint success;
    char info_log[1024] = {0};

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        assert(0); // TODO: ADD PROPER LOGGING.
        return 1;
    }
    return 0;
}
