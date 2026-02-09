#include "r_main.h"
#include "r_shader.h"
#include "data_vertices.c"
#include "glsl_shaders.c"
#include <assert.h>

int r_renderer_init(RendererContext* r)
{
    assert(r);

    r->shader_basic_3d.prog = shader_prog_create_from_memory(glsl_basic_vs, glsl_basic_fs);
    if (!r_shader_valid(r->shader_basic_3d)) {
        return 1;
    }
    r->shader_basic_3d.model = shader_get_uniform_location(r->shader_basic_3d.prog, "model");
    r->shader_basic_3d.view = shader_get_uniform_location(r->shader_basic_3d.prog, "view");
    r->shader_basic_3d.projection = shader_get_uniform_location(r->shader_basic_3d.prog, "projection");


}
