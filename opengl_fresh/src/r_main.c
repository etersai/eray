#include "r_main.h"
#include "r_shader.h"
#include "data_vertices.c"
#include "glsl_shaders.c"
#include <assert.h>

int r_renderer_init(RendererContext* r)
{
    assert(r);

    // SHADER BASIC
    r->shader_basic.prog = shader_prog_create_from_memory(glsl_basic_vs, glsl_basic_fs);
    if (!r_shader_valid(r->shader_basic)) {
        return 1;
    }
    r->shader_basic.model = shader_get_uniform_location(r->shader_basic.prog, "model");
    r->shader_basic.view = shader_get_uniform_location(r->shader_basic.prog, "view");
    r->shader_basic.projection = shader_get_uniform_location(r->shader_basic.prog, "projection");

    // basic model (teapot) shader

    shader_basic_model.prog = shader_prog_create_from_memory(glsl_teapot_vs, glsl_teapot_fs);
    if (!shader_valid(shader_basic_teapot)) {
        elog_abort("ABORT FOR NOW");
    }
    shader_basic_teapot.model = shader_get_uniform_location(shader_basic_teapot.prog, "model");
    shader_basic_teapot.view = shader_get_uniform_location(shader_basic_teapot.prog, "view");
    shader_basic_teapot.projection = shader_get_uniform_location(shader_basic_teapot.prog, "projection");
    shader_basic_teapot.color = shader_get_uniform_location(shader_basic_teapot.prog, "color");
}
