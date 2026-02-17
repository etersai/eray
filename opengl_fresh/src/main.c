#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "platform.h"
#include "r_main.h"
#include "r_opengl.h"
#include "r_camerka.h"
#include "r_shader.h"
#include "obj_loader.h"
#include "lamath.h"
#include "common/arena.h"
#include "common/str.h"
#include "common/types.h"

// VERY IMPORTANT TODOS //
// TODO: ADD ERROR CHECKS FOR GL STUFF.
// TODO: ADD AND ENABLE GL DEBUG FUNCTIONALITY.
// TODO: PROPER LOGGING FUNCTIONALITY
// TODO: SEPERATE PLATFORM LAYER

/////////////////////////////////////////////
// THIS GOES TO PLATFORM
global bool move_w;
global bool move_s;
global bool move_a;
global bool move_d;
global const float mouse_sens = 0.05f;
global const unsigned int SCR_WIDTH = 1920; // 16:9
global const unsigned int SCR_HEIGHT = 1080; 
//global const unsigned int SCR_WIDTH = 1280; // 16:9
//global const unsigned int SCR_HEIGHT = 720;
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // hackyy
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { move_w = true; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { move_s = true; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { move_a = true; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { move_d = true; }
}

global double last_x;
global double last_y;
global double mouse_dx;
global double mouse_dy;
global bool first_mouse = true;
//#define MOUSE_FLIP_Y
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
        return;
    }

    mouse_dx += xpos - last_x;
#ifdef MOUSE_FLIP_Y
    mouse_dy += ypos - last_y;
#else
    mouse_dy += last_y - ypos;
#endif /* INVERSE_MOUSE */

    last_x = xpos;
    last_y = ypos;
}



// END PLATFORM ////////////////////////////////////////

typedef enum {
    PIXEL_FORMAT_R8,
    PIXEL_FORMAT_RG8,
    PIXEL_FORMAT_RGB8,
    PIXEL_FORMAT_RGBA8,
} ImagePixelFormat;

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int num_channels;
    int format;
} CpuImage;

CpuImage cpu_image_load(const char* path)
{
    CpuImage img = {0};
    int width;
    int height;
    int num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);
    
    if (data) {

        img.data = data;
        img.width = width;
        img.height = height;
        img.num_channels = num_channels;

        switch (num_channels) 
        {
            case 1: img.format = PIXEL_FORMAT_R8;    break;
            case 2: img.format = PIXEL_FORMAT_RG8;   break;
            case 3: img.format = PIXEL_FORMAT_RGB8;  break;
            case 4: img.format = PIXEL_FORMAT_RGBA8; break;
            default: elog_abort("Unsupported channel count");
        }

        return img;
    }

    return img;
}

void cpu_image_unload(CpuImage* img)
{
    if (img != NULL && img->data != NULL) {
        stbi_image_free(img->data);
        img->data = NULL;
        img->width = 0;
        img->height = 0;
    }
}

Texture texture_load_from_path(const char* path)
{
    Texture texture = {0};
    CpuImage img = cpu_image_load(path);
    if (img.data != NULL) {
        texture = texture_create_from_memory(img.data, img.width, img.height, img.num_channels);
        cpu_image_unload(&img);
        return texture;
    }
    return texture;
}

void console_post_cwd(void)
{
    enum { CWD_PATH_MAX = 1024 };
    char buf[CWD_PATH_MAX];
    char* success = os_get_cwd(buf, sizeof(buf));
    if (success) {fprintf(stderr, "%s\n", buf);} 
    else {fprintf(stderr, "%s\n", "[err: getcwd failed]");}
}

void update_camera_input(Camerka* camera)
{
    assert(camera);
    camera->yaw   += mouse_dx * mouse_sens;
    camera->pitch += mouse_dy * mouse_sens;
    if (camera->pitch > 89.0f)  camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    mouse_dx = 0.0;
    mouse_dy = 0.0;

    vecf3 world_up = (vecf3){0.0f, 1.0f, 0.0f};
    vecf3 movement_vector = VECF3_ZERO;
    vecf3 orient = camerka_orientation(camera);
    camera->orientation = orient; // cache it.
    if (move_w) {
        vecf3_apply_add(&movement_vector, orient);
    }
    if (move_s) {
        vecf3_apply_sub(&movement_vector, orient);
    }
    if (move_a || move_d) {
        vecf3 right = vecf3_cross(orient, world_up);
        right = vecf3_norm(right);
        if (move_a) {
            right.x = -right.x;
            right.y = -right.y;
            right.z = -right.z;
        }
        vecf3_apply_add(&movement_vector, right);
    }
    if (vecf3_len(movement_vector) > 0.0f) {
        movement_vector = vecf3_norm(movement_vector);
        movement_vector = vecf3_scale(0.1f, movement_vector); // hackyyy
        vecf3_apply_add(&camera->pos, movement_vector);
    }
}


typedef struct {
    Fontek font;
    Texture texture_font;
    Texture texture_grass;
    TextureCubemap texture_skybox;
    GpuMeshIndexed mesh_teapot;
    Camerka camera;
    Arenka arena_obj_loading;
    Arenka arena_scratch;

    string8 player_pos;
    string8 player_orient;
    string8 time_fps;
    string8 time_dt;   

    f32 ambient_light_strength;
} ProgramContext;

internal inline void update_timing_strings(ProgramContext* ctx, u32 frames, f64 delta_time)
{
    ctx->time_fps = str8_fmt(&ctx->arena_scratch, "FPS: %d", frames);
    ctx->time_dt = str8_fmt(&ctx->arena_scratch, "DELTA TIME: %f", delta_time);
}

internal inline void update_camera_info_strings(ProgramContext* ctx)
{
    ctx->player_pos = str8_fmt(&ctx->arena_scratch,"[CAMERA POS] X: %f, Y: %f, Z: %f", ctx->camera.pos.x, ctx->camera.pos.y, ctx->camera.pos.z);
    ctx->player_orient = str8_fmt(&ctx->arena_scratch,"[CAMERA DIR] X: %f, Y: %f, Z: %f", ctx->camera.orientation.x, ctx->camera.orientation.y, ctx->camera.orientation.z); 
}

global ProgramContext program_ctx;
global RendererContext renderer_ctx;

int main(void)
{
    console_post_cwd();
    // Setup GLFW and OpenGL Context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Clonecraft", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }    
    glfwSwapInterval(1); // 1 VSYNC ON / 0 OFF

    /*******************/
    /* END BOILERPLATE */
    /*******************/
    
    program_ctx.arena_scratch = arenka_map(MB(8));
    if (program_ctx.arena_scratch.addr_start == NULL) { 
        elog_abort("Os failed at giving you memory xd");
    }
    program_ctx.arena_obj_loading = arenka_map(MB(8));
    if (program_ctx.arena_obj_loading.addr_start == NULL) { 
        elog_abort("Os failed at giving you memory xd");
    }

    // MEDIA LOADING
    // TODO: create temp texture for any failed to load texture [black/purple checkerboard pattern etc.]
    stbi_set_flip_vertically_on_load(true); // global state 
    
    const char* path_teapot = "./assets/models/teapot.obj";
    const char* path_grass = "./assets/textures/grass.jpg";
    const char* path_font = "./assets/textures/font.png";
    const char* paths_cubemap[] = {  
        "./assets/skybox/right.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        "./assets/skybox/left.jpg",   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        "./assets/skybox/top.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        "./assets/skybox/bottom.jpg", // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        "./assets/skybox/front.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        "./assets/skybox/back.jpg"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    }; 

    program_ctx.texture_grass = texture_load_from_path(path_grass);
    program_ctx.texture_font = texture_load_from_path(path_font); 
    program_ctx.texture_skybox = texture_cubemap_create_from_paths(paths_cubemap);

    if (!gl_texture_valid(program_ctx.texture_grass)) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", path_grass);
        abort();
    }
    if (!gl_texture_valid(program_ctx.texture_font)) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", path_font);
        abort();
    }
    if (!gl_texture_valid(program_ctx.texture_skybox)) {
        log_print_prefix("asset_load_error", "failed to load cubemap/skybox\n");
        abort();
    }

    // font
    font_create(&program_ctx.font, font_arial_white, program_ctx.texture_font);  
    
    // Rudimentary obj loading.
    obj_in_memory* obj_teapot = (obj_in_memory*)arenka_get_piece(&program_ctx.arena_obj_loading, sizeof(obj_in_memory));
    if (load_obj_from_path(obj_teapot, path_teapot) != 0) {
         log_print_prefix("asset_load_error", "failed to load '%s'\n", path_teapot);
         abort();
    }

    program_ctx.mesh_teapot = gpu_load_mesh_indexed(obj_teapot->vertices, obj_teapot->indices, 
            obj_teapot->v_count*sizeof(float), obj_teapot->i_count*sizeof(unsigned int), VERTEX_LAYOUT_POS);
    

    
    if (r_renderer_init(&renderer_ctx) != 0) {
        elog_abort("Renderer initialization failed!");
    }
    r_set_font(&renderer_ctx, &program_ctx.font);

    // GROUND precalculate planes positions.
    const int area_size = 20;
    vecf3 ground_translations[1600];
    int curr = 0;
    for (int z = -area_size; z < area_size; z++) {
        for (int x = -area_size; x < area_size; x++) {
            ground_translations[curr] = (vecf3){(float)x+0.5f, 0.0f, (float)z+0.5f};
            curr++;
        }
    }

    matf4x4 ground_transform = lamath_create_transform(VECF3_ZERO, (vecf3){deg_to_rad(-90.0f), 0.0f, 0.0f}, (vecf3){1.0f, 1.0f, 1.0f});
    shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.model, &ground_transform.col1.x);
    shader_set_3fv(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.offsets, 1600, &ground_translations[0].x); 

    // ANCHOR Transform
    matf4x4 transform_anchor = lamath_create_transform((vecf3){0.0f, 1.0f, 0.0f}, VECF3_ZERO, (vecf3){0.05f, 0.05f, 0.05f});

    // camera setup PROJECTION SET ONCE AT START !!
    vecf3 pos = (vecf3){-5.0f, 2.0f, 0.0f};
    camerka_set_pos(program_ctx.camera, pos);
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_lighting.prog, renderer_ctx.shader_lighting.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_basic_3d_color.prog, renderer_ctx.shader_basic_3d_color.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_skybox.prog, renderer_ctx.shader_skybox.projection, &projection.col1.x);

    // GL CALLS BUT WRAPPED 
    gl_enable_depth_test();
    gl_set_clear_color((Colorek){0.5f, 0.5f, 0.5f, 1.0f});

    // "should i put it here?" section
    program_ctx.ambient_light_strength = 0.1f;
    shader_set_float(renderer_ctx.shader_lighting.prog,
                     renderer_ctx.shader_lighting.ambient_strength,
                     program_ctx.ambient_light_strength);

    u32 frames = 0;
    f64 delta_time;
    double curr_time;
    double prev_time = glfwGetTime();
    double prev_frame_time = prev_time;
    while (!glfwWindowShouldClose(window)) {
        // TIMING STUFF.
        curr_time = glfwGetTime();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        frames++;
        if (curr_time - prev_frame_time >= 1.0) {
            update_timing_strings(&program_ctx, frames, delta_time);
            frames = 0;
            prev_frame_time = curr_time;
        }

        // HANDLE INPUT
        processInput(window);
        update_camera_input(&program_ctx.camera);

        update_camera_info_strings(&program_ctx);

        // UPDATE VIEW MATRICES.
        matf4x4 view = camerka_view_matrix(&program_ctx.camera); // you can use shader uniforms that can be shared across multiple shaders.
        shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.view, &view.col1.x);
        shader_set_mat4(renderer_ctx.shader_lighting.prog, renderer_ctx.shader_lighting.view, &view.col1.x);
        shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.view, &view.col1.x);
        shader_set_mat4(renderer_ctx.shader_basic_3d_color.prog, renderer_ctx.shader_basic_3d_color.view, &view.col1.x);
        matf4x4 view_no_translation = view;
        view_no_translation.col4.x = 0.0f; // zero out translation
        view_no_translation.col4.y = 0.0f;
        view_no_translation.col4.z = 0.0f;
        shader_set_mat4(renderer_ctx.shader_skybox.prog, renderer_ctx.shader_skybox.view, &view_no_translation.col1.x);

        // RENDER START
        r_begin_frame(&renderer_ctx);
        r_draw_text_str8(&renderer_ctx, 0.0f, 0.0f, str8_lit("CLONECRAFT V0.001")); 
        r_draw_text_str8(&renderer_ctx, 0.0f, 36.0f, program_ctx.time_fps); 
        r_draw_text_str8(&renderer_ctx, 0.0f, 72.0f, program_ctx.time_dt); 
        r_draw_text_str8(&renderer_ctx, 0.0f, 108.0f, program_ctx.player_pos); 
        r_draw_text_str8(&renderer_ctx, 0.0f, 144.0f, program_ctx.player_orient); 

        r_draw_skybox(&renderer_ctx, program_ctx.texture_skybox);        

        // ground
        shader_prog_use(renderer_ctx.shader_instanced.prog);
        glBindTexture(GL_TEXTURE_2D, program_ctx.texture_grass.id);
        glBindVertexArray(renderer_ctx.mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, renderer_ctx.mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);

        r_draw_anchor(&renderer_ctx, &transform_anchor);

        // LIGHT FIRST CONTACT
        matf4x4 bulb_trans = lamath_create_transform((vecf3){4.0f, 2.0f, 1.0f}, VECF3_ZERO, (vecf3){0.5f, 0.5f, 0.5f});
        Colorek bulb_color = (Colorek){1.0f, 1.0f, 1.0f, 1.0f};
        r_draw_cube(&renderer_ctx, &bulb_trans, bulb_color);

        matf4x4 receiver_trans = lamath_create_transform((vecf3){6.0f, 3.0f, 1.0f}, VECF3_ZERO, (vecf3){0.5f, 0.5f, 0.5f});
        Colorek receiver_color = (Colorek){0.5f, 1.0f, 0.3f, 1.0f}; // NOTE colore 4 components shader set vec3 inside func.
        r_draw_cube_light_receiver(&renderer_ctx, &receiver_trans, receiver_color, bulb_color);

        // teapot
        local_persist float rotacja = 0.0f;
        local_persist const float teapot_color[] = {1.0f, 0.5f, 0.31f, 1.0f}; 
        matf4x4 s2 = matf4x4_I_give();
        matf4x4 r = matf4x4_I_give();
        matf4x4 t2 = matf4x4_translate_give((vecf3){0.0f, 0.0f, 0.0f});
        matf4x4_scale_set(&s2, 1.0f, 1.0f, 1.0f);
        matf4x4_rot_y(&r, rotacja);
        matf4x4 full2 = matf4x4_I_give();
        matf4x4_mul_3(&full2, &t2, &r, &s2);
        rotacja += 0.002f;

        r_draw_mesh_indexed(&renderer_ctx, program_ctx.mesh_teapot, &full2, *(Colorek*)&teapot_color[0]);

        matf4x4 tfull2 = lamath_create_transform((vecf3){0.0f, 5.0f, 0.0f}, (vecf3){0.5f, 0.5f, 0.5f}, (vecf3){1.0f, 1.0f, 1.0f});
        r_draw_mesh_indexed(&renderer_ctx, program_ctx.mesh_teapot, &tfull2, *(Colorek*)&teapot_color[0]);
    
        r_flush_text(&renderer_ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
        move_w = false;
        move_s = false;
        move_a = false;
        move_d = false;
    }
    r_renderer_shutdown(&renderer_ctx);    
    arenka_unmap(&program_ctx.arena_obj_loading);
    arenka_unmap(&program_ctx.arena_scratch);
    glfwTerminate();
    return 0;
}
