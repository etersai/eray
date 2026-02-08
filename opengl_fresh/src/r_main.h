#ifndef R_MAIN_H_
#define R_MAIN_H_

#include <glad/glad.h>
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
    ShaderBasic shader_basic;
    ShaderBasic shader_basic_teapot;
    ShaderBasic shader_font;
    ShaderInstanced shader_instanced;
    ShaderSkybox shader_skybox;

    GpuMeshIndexed mesh_quad;
    GpuMeshIndexed mesh_cube;
    GpuMeshIndexed mesh_anchor;
    GpuMeshIndexed mesh_model;
    GpuMeshSimple  mesh_skybox; // -1 to 1 cube at (0,0,0) [36 vertices, only pos]
                                
    Texture texture_font_arial_white;
    Texture texture_grass;
    TextureCubemap texture_skybox;
} Renderek;

#define shader_valid(shader) ((shader).prog.id != 0)

#endif /* R_MAIN_H_ */
