#ifndef R_MAIN_H_
#define R_MAIN_H_

#include <glad/glad.h>
#include "r_opengl.h"
#include "r_shader.h"

typedef GLint uniform;

typedef struct {
    ShaderProgram prog;
    uniform model;
    uniform view;
    uniform projection;
    uniform offsets;
} ShaderInstanced;

typedef struct {
    ShaderProgram prog;
    uniform model;
    uniform view;
    uniform projection;
    uniform color;
} ShaderBasic;

typedef struct {
    ShaderProgram prog;
    uniform view;
    uniform projection;
} ShaderSkybox;

typedef struct {
    ShaderBasic shader_basic_3d;
    ShaderBasic shader_font;
    ShaderInstanced shader_instanced;
    ShaderSkybox shader_skybox;

    GpuMeshIndexed mesh_quad;
    GpuMeshIndexed mesh_cube;
    GpuMeshIndexed mesh_anchor;
    GpuMeshSimple  mesh_skybox; // -1 to 1 cube at (0,0,0) [36 vertices, only pos]
} RendererContext;

#define r_shader_valid(shader) ((shader).prog.id != 0)

int r_renderer_init(RendererContext* r);
void r_renderer_shutdown(RendererContext* r);

#endif /* R_MAIN_H_ */
