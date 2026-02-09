#include "r_main.h"
#include "r_shader.h"
#include "data_vertex.c"
#include <assert.h>

int r_renderer_init(RendererContext* renderer)
{
    assert(renderer);
    
    // SHADERS //
    // basic shader.  
    shader_basic.prog = shader_prog_create_from_memory(glsl_basic_vs, glsl_basic_fs);
    if (!shader_valid(shader_basic)) {
        elog_abort("ABORT FOR NOW"); 
    }
    shader_basic.model = shader_get_uniform_location(shader_basic.prog, "model");
    shader_basic.view = shader_get_uniform_location(shader_basic.prog, "view");
    shader_basic.projection = shader_get_uniform_location(shader_basic.prog, "projection");

    // shader font
    shader_font.prog = shader_prog_create_from_memory(glsl_font_vs, glsl_font_fs);
    if (!shader_valid(shader_font)) {
        elog_abort("ABORT FOR NOW"); 
    }

    // basic model (teapot) shader
    shader_basic_teapot.prog = shader_prog_create_from_memory(glsl_teapot_vs, glsl_teapot_fs);
    if (!shader_valid(shader_basic_teapot)) {
        elog_abort("ABORT FOR NOW");
    }
    shader_basic_teapot.model = shader_get_uniform_location(shader_basic_teapot.prog, "model");
    shader_basic_teapot.view = shader_get_uniform_location(shader_basic_teapot.prog, "view");
    shader_basic_teapot.projection = shader_get_uniform_location(shader_basic_teapot.prog, "projection");
    shader_basic_teapot.color = shader_get_uniform_location(shader_basic_teapot.prog, "color");

    // skybox shader.
    shader_skybox.prog = shader_prog_create_from_memory(glsl_skybox_vs, glsl_skybox_fs);
    if (shader_skybox.prog.id == 0) {
        elog_abort("SHADER COMPILATION FAILED");
    }
    shader_skybox.view = shader_get_uniform_location(shader_skybox.prog, "view");
    shader_skybox.projection = shader_get_uniform_location(shader_skybox.prog, "projection");

    // ground shader.
    shader_instanced.prog = shader_prog_create_from_memory(glsl_instanced_vs, glsl_instanced_fs); 
    if (shader_instanced.prog.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    shader_instanced.model = shader_get_uniform_location(shader_instanced.prog, "model");
    shader_instanced.view = shader_get_uniform_location(shader_instanced.prog, "view");
    shader_instanced.projection = shader_get_uniform_location(shader_instanced.prog, "projection");
    shader_instanced.offsets = shader_get_uniform_location(shader_instanced.prog, "offsets");

    // LOAD MESHES
    mesh_quad = gpu_load_mesh_quad(quad_verts, quad_indices, sizeof(quad_verts), sizeof(quad_indices));
    mesh_anchor = gpu_load_mesh_anchor(anchor_vertices, anchor_indices, sizeof(anchor_vertices), sizeof(anchor_indices));
    mesh_skybox = gpu_load_mesh_simple_1attr(skyboxVertices, sizeof(skyboxVertices));
    mesh_model = gpu_load_mesh_model(teapot_obj.vertices, teapot_obj.indices, teapot_obj.v_count*sizeof(float), teapot_obj.i_count*sizeof(unsigned int));
    mesh_cube = gpu_load_mesh_3attr(cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices));


}
