#include "r_main.h"
#include "font.h"
#include "lamath.h"
#include "r_opengl.h"
#include "r_shader.h"
#include "r_glsl_shaders.c"
#include "data_vertices.c"
#include "common/types.h"
#include "common/str.h"
#include <assert.h>

// kinda sus.
internal void shader_basic_get_mvp_uniform_locations(ShaderBasic* basic, ShaderProgram program);

void r_begin_frame(RendererContext* r)
{
    assert(r);
    la_unused(r);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r_draw_skybox(RendererContext* r, TextureCubemap texture)
{
    assert(r);
    glDepthMask(GL_FALSE);
    shader_prog_use(r->shader_skybox.prog);
    glBindVertexArray(r->mesh_skybox.VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void r_draw_mesh_indexed(RendererContext* r, GpuMeshIndexed mesh, const matf4x4* transform, Colorek color)
{
    assert(r);
    shader_prog_use(r->shader_basic_3d.prog);
    shader_set_mat4(r->shader_basic_3d.prog, r->shader_basic_3d.model, &transform->col1.x);
    shader_set_vec4(r->shader_basic_3d.prog, r->shader_basic_3d.color, &color.r);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void r_draw_anchor(RendererContext* r, const matf4x4* transform)
{
    shader_prog_use(r->shader_basic_3d_color.prog);
    shader_set_mat4(r->shader_basic_3d_color.prog, r->shader_basic_3d_color.model, &transform->col1.x);
    glBindVertexArray(r->mesh_anchor.VAO);
    glDrawElements(GL_TRIANGLES, r->mesh_anchor.index_count, GL_UNSIGNED_INT, 0);
}

void r_draw_quad(RendererContext* r, float x, float y, Texture texture)
{
    // remap // viewport to ndc
    float norm_x = x/r->viewport_width; // hacky
    float norm_y = y/r->viewport_height; // wacky
    float ndc_x = norm_x * 2 - 1;
    float ndc_y = -(norm_y * 2 - 1); // flip for it to match opengl.
}

void r_draw_block(RendererContext* r, const matf4x4* transform, Texture texture)
{
    
}

void r_draw_cube(RendererContext* r, const matf4x4* transform, Colorek color)
{
    shader_prog_use(r->shader_basic_3d.prog);
    shader_set_mat4(r->shader_basic_3d.prog, r->shader_basic_3d.model, &transform->col1.x);
    shader_set_vec4(r->shader_basic_3d.prog, r->shader_basic_3d.color, &color.r);
    glBindVertexArray(r->mesh_cube.VAO);
    glDrawElements(GL_TRIANGLES, r->mesh_cube.index_count, GL_UNSIGNED_INT, 0);
}

void r_draw_cube_light_receiver(RendererContext* r, const matf4x4* transform, Colorek color, Colorek light_color)
{
    shader_prog_use(r->shader_lighting.prog);
    shader_set_mat4(r->shader_lighting.prog, r->shader_lighting.model, &transform->col1.x);
    shader_set_vec3(r->shader_lighting.prog, r->shader_lighting.object_color, &color.r);
    shader_set_vec3(r->shader_lighting.prog, r->shader_lighting.light_color, &light_color.r);
    glBindVertexArray(r->mesh_cube.VAO);
    glDrawElements(GL_TRIANGLES, r->mesh_cube.index_count, GL_UNSIGNED_INT, 0);
}

int r_renderer_init(RendererContext* r, int viewport_width, int viewport_height)
{
    assert(r);
    assert(viewport_width > 0);
    assert(viewport_height > 0);

    r->viewport_width = viewport_width;
    r->viewport_height = viewport_height;

    /////////////
    // SHADERS 

    // basic 2d
    r->shader_basic_2d.prog = shader_prog_create_from_memory(glsl_basic_2d_vs, glsl_basic_2d_fs);
    if (!r_shader_valid(r->shader_basic_2d)) {
        return 1;
    }
    shader_basic_get_mvp_uniform_locations(&r->shader_basic_2d, r->shader_basic_2d.prog);

    // basic 3d
    r->shader_basic_3d.prog = shader_prog_create_from_memory(glsl_basic_3d_vs, glsl_basic_3d_fs);
    if (!r_shader_valid(r->shader_basic_3d)) {
        return 1;
    }
    shader_basic_get_mvp_uniform_locations(&r->shader_basic_3d, r->shader_basic_3d.prog);
    r->shader_basic_3d.color = shader_get_uniform_location(r->shader_basic_3d.prog, "color");

    // basic 3d color attribute
    r->shader_basic_3d_color.prog = shader_prog_create_from_memory(glsl_basic_3d_color_vs, glsl_basic_3d_color_fs);
    if (!r_shader_valid(r->shader_basic_3d_color)) {
        return 1;
    }
    shader_basic_get_mvp_uniform_locations(&r->shader_basic_3d_color, r->shader_basic_3d_color.prog);

    // lighting
    r->shader_lighting.prog = shader_prog_create_from_memory(glsl_basic_3d_vs, glsl_light_fs);
    if (!r_shader_valid(r->shader_lighting)) {
        return 1;
    }
    r->shader_lighting.model = shader_get_uniform_location(r->shader_lighting.prog, "model");
    r->shader_lighting.view = shader_get_uniform_location(r->shader_lighting.prog, "view");
    r->shader_lighting.projection = shader_get_uniform_location(r->shader_lighting.prog, "projection"); 
    r->shader_lighting.ambient_strength = shader_get_uniform_location(r->shader_lighting.prog, "ambientStrength");
    r->shader_lighting.light_color = shader_get_uniform_location(r->shader_lighting.prog, "lightColor");
    r->shader_lighting.object_color = shader_get_uniform_location(r->shader_lighting.prog, "objectColor"); 
    
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

    /////////
    // Mesh
    r->mesh_skybox = gpu_load_mesh_simple(skybox_vertices, sizeof(skybox_vertices));
    r->mesh_quad = gpu_load_mesh_indexed(quad_vertices, quad_indices, sizeof(quad_vertices), sizeof(quad_indices), VERTEX_LAYOUT_POS_TEX);
    r->mesh_anchor = gpu_load_mesh_indexed(anchor_vertices, anchor_indices, sizeof(anchor_vertices), sizeof(anchor_indices), VERTEX_LAYOUT_POS_COLOR);
    r->mesh_cube = gpu_load_mesh_indexed(cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices), VERTEX_LAYOUT_CUBE_POS);
    r->mesh_block = gpu_load_mesh_indexed(cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices), VERTEX_LAYOUT_POS_NORM_TEX);

    ///////////////////
    // Text rendering 
    r->text_gpu_buffer = gpu_p_mapped_buffer_create(TEXT_MAX_GLYPHS*TEXT_FLOATS_PER_GLYPH*sizeof(float));
    assert(gl_buffer_valid(r->text_gpu_buffer));
    r->text_vao = gpu_vao_create_for_text_buffer(r->text_gpu_buffer.VBO); 
    assert(r->text_vao != 0);
    
    return 0;
}

void r_renderer_shutdown(RendererContext* r)
{
    assert(r);
    gpu_p_mapped_buffer_destroy(&r->text_gpu_buffer);

    shader_prog_delete(r->shader_basic_2d.prog);
    shader_prog_delete(r->shader_basic_3d.prog);
    shader_prog_delete(r->shader_lighting.prog);
    shader_prog_delete(r->shader_skybox.prog);
    shader_prog_delete(r->shader_instanced.prog);
    shader_prog_delete(r->shader_font.prog);

    gpu_delete_mesh_indexed(r->mesh_quad); 
    gpu_delete_mesh_indexed(r->mesh_cube); 
    gpu_delete_mesh_indexed(r->mesh_anchor); 
    gpu_delete_mesh_indexed(r->mesh_block);
    gpu_delete_mesh_simple(r->mesh_skybox); 
}

internal void shader_basic_get_mvp_uniform_locations(ShaderBasic* basic, ShaderProgram program)
{
    basic->model = shader_get_uniform_location(program, "model");
    basic->view = shader_get_uniform_location(program, "view");
    basic->projection = shader_get_uniform_location(program, "projection");
}

void r_set_font(RendererContext* r, Fontek* font)
{
    assert(r);
    assert(font);
    r->text_font = font;
}

void r_draw_text_str8(RendererContext* r, float x, float y, string8 str)
{// one char = 24 floats = 96 bytes.
    assert(r); 
    assert(r->text_font);

    // remap
    float norm_x = x/r->viewport_width; // hacky
    float norm_y = y/r->viewport_height; // wacky
    float start_ndc_x = norm_x * 2 - 1;
    float start_ndc_y = -(norm_y * 2 - 1); // flip for it to match opengl.
 
    size_t byte_offset_into_buffer = sizeof(float) * r->text_fv_count;
    unsigned int internal_text_fv_count = 0;

    for (u64 i = 0; i < str.size; i++) {
        internal_font_char character = r->text_font->metadata.characters[0]; // 0 for space by default.
        char c = (char)str.bytes[i];    
        if (c >= ' ' && c <= '~') { // printable ascii range
            character = r->text_font->metadata.characters[(int)c-32];
        }
        
        float uv_width = (float)character.width/r->text_font->texture.width;
        float uv_height = (float)character.height/r->text_font->texture.height;
        float uv_x = (float)character.x/r->text_font->texture.width;
        float uv_y = (float)character.y/r->text_font->texture.height;

        float char_width_norm = 2*((float)character.width / r->viewport_width); // hacky
        float char_height_norm = 2*((float)character.height / r->viewport_height); // wacky

        // Generate 2 triangles 
        // box top left. 0
        r->text_cpu_buffer[internal_text_fv_count] = start_ndc_x; 
        r->text_cpu_buffer[internal_text_fv_count+1] = start_ndc_y;
        r->text_cpu_buffer[internal_text_fv_count+2] = uv_x;
        r->text_cpu_buffer[internal_text_fv_count+3] = uv_y;  

        // box bottom left. 1
        r->text_cpu_buffer[internal_text_fv_count+4] = start_ndc_x;
        r->text_cpu_buffer[internal_text_fv_count+5] = start_ndc_y-char_height_norm;
        r->text_cpu_buffer[internal_text_fv_count+6] = uv_x;
        r->text_cpu_buffer[internal_text_fv_count+7] = uv_y+uv_height;

        // box bottom right. 2
        r->text_cpu_buffer[internal_text_fv_count+8]  = start_ndc_x+char_width_norm;
        r->text_cpu_buffer[internal_text_fv_count+9]  = start_ndc_y-char_height_norm; 
        r->text_cpu_buffer[internal_text_fv_count+10] = uv_x+uv_width; 
        r->text_cpu_buffer[internal_text_fv_count+11] = uv_y+uv_height; 


        // box bottom right. 2
        r->text_cpu_buffer[internal_text_fv_count+12]  = start_ndc_x+char_width_norm;
        r->text_cpu_buffer[internal_text_fv_count+13]  = start_ndc_y-char_height_norm; 
        r->text_cpu_buffer[internal_text_fv_count+14] = uv_x+uv_width; 
        r->text_cpu_buffer[internal_text_fv_count+15] = uv_y+uv_height; 

        // box top right. 3 
        r->text_cpu_buffer[internal_text_fv_count+16] = start_ndc_x+char_width_norm; 
        r->text_cpu_buffer[internal_text_fv_count+17] = start_ndc_y; 
        r->text_cpu_buffer[internal_text_fv_count+18] = uv_x+uv_width; 
        r->text_cpu_buffer[internal_text_fv_count+19] = uv_y; 

        // top 
        r->text_cpu_buffer[internal_text_fv_count+20] = start_ndc_x;
        r->text_cpu_buffer[internal_text_fv_count+21] = start_ndc_y;
        r->text_cpu_buffer[internal_text_fv_count+22] = uv_x;
        r->text_cpu_buffer[internal_text_fv_count+23] = uv_y;

        start_ndc_x += char_width_norm;
        internal_text_fv_count+=TEXT_FLOATS_PER_GLYPH;
    }

    gpu_p_mapped_buffer_write(&r->text_gpu_buffer,
                              byte_offset_into_buffer,
                              internal_text_fv_count*sizeof(float),
                              &r->text_cpu_buffer);
     
    r->text_fv_count+=internal_text_fv_count;
}

void r_flush_text(RendererContext *r)
{
    assert(r);
    assert(r->text_font);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_prog_use(r->shader_font.prog);
    glBindVertexArray(r->text_vao);
    glBindTexture(GL_TEXTURE_2D, r->text_font->texture.id);
    glDrawArrays(GL_TRIANGLES, 0, r->text_fv_count);

    glDisable(GL_BLEND);
    r->text_fv_count = 0;
}
