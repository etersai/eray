#include "r_main.h"
#include "r_shader.h"
#include "r_glsl_shaders.c"
#include "data_vertices.c"
#include <assert.h>

int r_renderer_init(RendererContext* r)
{
    assert(r);
    
    // basic 3d
    r->shader_basic_3d.prog = shader_prog_create_from_memory(glsl_basic_3d_vs, glsl_basic_3d_fs);
    if (!r_shader_valid(r->shader_basic_3d)) {
        return 1;
    }
    r->shader_basic_3d.model = shader_get_uniform_location(r->shader_basic_3d.prog, "model");
    r->shader_basic_3d.view = shader_get_uniform_location(r->shader_basic_3d.prog, "view");
    r->shader_basic_3d.projection = shader_get_uniform_location(r->shader_basic_3d.prog, "projection");
    r->shader_basic_3d.color = shader_get_uniform_location(r->shader_basic_3d.prog, "color");
    
    // instanced
    r->shader_instanced.prog = shader_prog_create_from_memory(glsl_instanced_vs, glsl_instanced_fs);
    if (!r_shader_valid(r->shader_instanced)) {
        return 1;
    }
    r->shader_instanced.model = shader_get_uniform_location(r->shader_instanced.prog, "model");
    r->shader_instanced.view = shader_get_uniform_location(r->shader_instanced.prog, "view");
    r->shader_instanced.projection = shader_get_uniform_location(r->shader_instanced.prog, "projection");
    r->shader_instanced.offsets = shader_get_uniform_location(r->shader_instanced.prog, "offsets");
    
    // skybox
    r->shader_skybox.prog = shader_prog_create_from_memory(glsl_skybox_vs, glsl_skybox_fs);
    if (!r_shader_valid(r->shader_skybox)) {
        return 1;
    }
    r->shader_skybox.view = shader_get_uniform_location(r->shader_skybox.prog, "view");
    r->shader_skybox.projection = shader_get_uniform_location(r->shader_skybox.prog, "projection");

    // font
    r->shader_font.prog = shader_prog_create_from_memory(glsl_font_vs, glsl_font_fs); 
    if (!r_shader_valid(r->shader_font)) {
        return 1;
    }
    
    return 0;
}

void r_renderer_shutdown(RendererContext* r)
{
    assert(r);
    shader_prog_delete(r->shader_basic_3d.prog);
    shader_prog_delete(r->shader_skybox.prog);
    shader_prog_delete(r->shader_instanced.prog);
    shader_prog_delete(r->shader_font.prog);
}
