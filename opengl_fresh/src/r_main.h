#ifndef R_MAIN_H_
#define R_MAIN_H_

#include <glad/glad.h>
#include "r_opengl.h"
#include "r_shader.h"
#include "font.h"
#include "lamath.h"
#include "common/str.h"
#include "common/types.h"

// Frustum culling solution:
// CPU-side test: For each quad, check if its bounding box intersects the view frustum.
// Only send visible quads to GPU.

// Depth mask info.
//Fragments still test against the depth buffer (can still be rejected)
//But passing fragments don't update the depth buffer
//The depth values stay whatever they were before

#define TEXT_MAX_GLYPHS (16384)
#define TEXT_FLOATS_PER_GLYPH (24)

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
    uniform model;
    uniform view;
    uniform projection;
    uniform object_color;
    uniform light_color;
    uniform ambient_strength;
} ShaderLighting;

typedef struct {
    ShaderProgram prog;
    uniform view;
    uniform projection;
} ShaderSkybox;

typedef struct {
    ShaderBasic     shader_basic_3d;
    ShaderBasic     shader_basic_3d_color;
    ShaderBasic     shader_font;
    ShaderLighting  shader_lighting;
    ShaderInstanced shader_instanced;
    ShaderSkybox    shader_skybox;

    GpuMeshIndexed mesh_quad;
    GpuMeshIndexed mesh_cube;
    GpuMeshIndexed mesh_block; // cube [pos|norm|tex]
    GpuMeshIndexed mesh_anchor;
    GpuMeshSimple  mesh_skybox; // -1 to 1 cube at (0,0,0) [36 vertices, only pos]
                                
    Fontek*          text_font;
    GLuint           text_vao;
    float            text_cpu_buffer[TEXT_MAX_GLYPHS*TEXT_FLOATS_PER_GLYPH]; 
    GpuPMappedBuffer text_gpu_buffer;                           
    unsigned int     text_fv_count; 
} RendererContext;

#define r_shader_valid(shader) ((shader).prog.id != 0)

void r_begin_frame(RendererContext* r);
void r_draw_text(RendererContext* r, float x, float y, const char* text);
void r_draw_text_str8(RendererContext* r, float x, float y, string8 str);
void r_flush_text(RendererContext* r);
void r_draw_skybox(RendererContext* r, TextureCubemap texture);
void r_draw_anchor(RendererContext* r, const matf4x4* transform);
void r_draw_cube(RendererContext* r, const matf4x4* transform, Colorek color);
void r_draw_cube_light_receiver(RendererContext* r, const matf4x4* transform, Colorek color, Colorek light_color);
void r_draw_mesh_indexed(RendererContext* r, GpuMeshIndexed mesh, const matf4x4* transform, Colorek color);
int r_renderer_init(RendererContext* r);
void r_set_font(RendererContext* r, Fontek* font);
void r_renderer_shutdown(RendererContext* r);

#endif /* R_MAIN_H_ */
