#ifndef R_MAIN_H_
#define R_MAIN_H_

#include <glad/glad.h>
#include "r_opengl.h"
#include "r_shader.h"

// Frustum culling solution:
// CPU-side test: For each quad, check if its bounding box intersects the view frustum.
// Only send visible quads to GPU.

// Depth mask info.
//Fragments still test against the depth buffer (can still be rejected)
//But passing fragments don't update the depth buffer
//The depth values stay whatever they were before

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

// HELP.
// void r_draw_text_immediate(Fontek* font, float x, float y, const char* text)
// {
//     // ... existing setup code ...
//
//     fv_count = 0;
//
//     float norm_x = x/SCR_WIDTH;
//     float norm_y = y/SCR_HEIGHT;
//     float pen_x_ndc = norm_x * 2 - 1;
//     float pen_y_ndc = -(norm_y * 2 - 1);
//
//     char* p = text;
//     while(*p != '\0') {
//         internal_font_char character = font->metadata.characters[0];
//         char c = *p;    
//         if (c >= ' ' && c <= '~') {
//             character = font->metadata.characters[(int)c-32];
//         }
//
//         // Calculate UV coordinates
//         float uv_width = (float)character.width / font->texture.width;
//         float uv_height = (float)character.height / font->texture.height;
//         float uv_x = (float)character.x / font->texture.width;
//         float uv_y = (float)character.y / font->texture.height;
//
//         // Convert glyph dimensions to NDC
//         float char_width_ndc = ((float)character.width / SCR_WIDTH) * 2.0f;
//         float char_height_ndc = ((float)character.height / SCR_HEIGHT) * 2.0f;
//
//         // IMPORTANT: Apply origin offsets for proper alignment
//         float origin_x_ndc = ((float)character.originX / SCR_WIDTH) * 2.0f;
//         float origin_y_ndc = ((float)character.originY / SCR_HEIGHT) * 2.0f;
//
//         // Calculate actual glyph position (pen position + bearing offset)
//         float glyph_x = pen_x_ndc + origin_x_ndc;
//         float glyph_y = pen_y_ndc - origin_y_ndc; // Subtract because Y is inverted
//
//         // Generate triangles at the adjusted position
//         // Triangle 1: TL, BL, BR
//         cpu_text_vertices[fv_count+0] = glyph_x;
//         cpu_text_vertices[fv_count+1] = glyph_y;
//         cpu_text_vertices[fv_count+2] = uv_x;
//         cpu_text_vertices[fv_count+3] = uv_y;
//
//         cpu_text_vertices[fv_count+4] = glyph_x;
//         cpu_text_vertices[fv_count+5] = glyph_y - char_height_ndc;
//         cpu_text_vertices[fv_count+6] = uv_x;
//         cpu_text_vertices[fv_count+7] = uv_y + uv_height;
//
//         cpu_text_vertices[fv_count+8] = glyph_x + char_width_ndc;
//         cpu_text_vertices[fv_count+9] = glyph_y - char_height_ndc;
//         cpu_text_vertices[fv_count+10] = uv_x + uv_width;
//         cpu_text_vertices[fv_count+11] = uv_y + uv_height;
//
//         // Triangle 2: BR, TR, TL
//         cpu_text_vertices[fv_count+12] = glyph_x + char_width_ndc;
//         cpu_text_vertices[fv_count+13] = glyph_y - char_height_ndc;
//         cpu_text_vertices[fv_count+14] = uv_x + uv_width;
//         cpu_text_vertices[fv_count+15] = uv_y + uv_height;
//
//         cpu_text_vertices[fv_count+16] = glyph_x + char_width_ndc;
//         cpu_text_vertices[fv_count+17] = glyph_y;
//         cpu_text_vertices[fv_count+18] = uv_x + uv_width;
//         cpu_text_vertices[fv_count+19] = uv_y;
//
//         cpu_text_vertices[fv_count+20] = glyph_x;
//         cpu_text_vertices[fv_count+21] = glyph_y;
//         cpu_text_vertices[fv_count+22] = uv_x;
//         cpu_text_vertices[fv_count+23] = uv_y;
//
//         // ADVANCE THE PEN: move by the full character width (the "advance" value)
//         // If you have an 'advance' field, use that instead
//         pen_x_ndc += char_width_ndc + origin_x_ndc; // width + bearing for proper spacing
//
//         fv_count += 24;
//         p++;
//     }
//
//     // ... rest of your drawing code ...
// }
