#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "platform.h"
#include "r_main.h"
#include "r_opengl.h"
#include "r_camerka.h"
#include "r_shader.h"
#include "obj_loader.h"
#include "lamath.h"
#include "data_font.h"

// VERY IMPORTANT TODOS //
// TODO: ADD ERROR CHECKS FOR GL STUFF.
// TODO: ADD AND ENABLE GL DEBUG FUNCTIONALITY.
// TODO: PROPER LOGGING FUNCTIONALITY
// TODO: SEPERATE PLATFORM LAYER

// credit Ryan Fleury
#define global        static 
#define internal      static
#define local_persist static

/////////////////////////////////////////////
// THIS GOES TO PLATFORM
global bool move_w;
global bool move_s;
global bool move_a;
global bool move_d;
global const float mouse_sens = 0.1f;
global const unsigned int SCR_WIDTH = 1280; // 16:9
global const unsigned int SCR_HEIGHT = 720; 
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

typedef struct {
    internal_font_data metadata;
    Texture texture;
} Fontek;

int font_create(Fontek* font, internal_font_data font_metadata, Texture texture)
{
    assert(font != NULL);
    assert(font_metadata.name != NULL); // at least some check XD. 
    assert(gl_texture_valid(texture));
    font->metadata = font_metadata;
    font->texture = texture;
    return 0;
}

// DRAW FONT STUFF //
////////////////////
#define TEXT_MAX_BYTES (1024*512) // 0.5kib
// relic of the ancient wisdom.
//static unsigned char cpu_text_vertices[TEXT_MAX_BYTES];
static float cpu_text_vertices[TEXT_MAX_BYTES/sizeof(float)]; // 5461.33 CHARS.                                                //
unsigned int fv_count = 0; 
static GLuint vao_text;
static GpuPMappedBuffer gpu_pmbuffer_text_vertices;
void r_draw_text_immediate(Fontek* font, float x, float y, const char* text)
{
    assert(font);
    assert(gl_texture_valid(font->texture));

    // kickstart it's self kinda thing.
    if (!gl_buffer_valid(gpu_pmbuffer_text_vertices)) { // if not created
        gpu_pmbuffer_text_vertices = gpu_p_mapped_buffer_create(TEXT_MAX_BYTES);
        assert(gl_buffer_valid(gpu_pmbuffer_text_vertices));
        vao_text = gpu_vao_create_for_text_buffer(gpu_pmbuffer_text_vertices.VBO);
        assert(vao_text != 0);
    }

    // struct uv { // you can create struct inside functions :D
    //     float x;
    //     float y;
    //     float w;
    //     float h;
    // };

    // remap
    float norm_x = x/SCR_WIDTH;
    float norm_y = y/SCR_HEIGHT;
    float start_ndc_x = norm_x * 2 - 1;
    float start_ndc_y = -(norm_y * 2 - 1); // flip for it to match opengl.
    
    size_t byte_buffer_offset = sizeof(float) * fv_count;

    char* p = text;
    while(*p != '\0') { // points to this null termination after loop
        internal_font_char character = font->metadata.characters[0]; // 0 for space by default.
        char c = *p;    
        if (c >= ' ' && c <= '~') { // printable ascii range
            character = font_arial_white.characters[(int)c-32];
        }
        
        float uv_width = (float)character.width/font->texture.width;
        float uv_height = (float)character.height/font->texture.height;
        float uv_x = (float)character.x/font->texture.width;
        float uv_y = (float)character.y/font->texture.height;


        float char_width_norm = 2*((float)character.width / SCR_WIDTH);
        float char_height_norm = 2*((float)character.height / SCR_HEIGHT);
        // one char = 24 floats = 96 bytes.

        // Generate 2 triangles 
        // box top left. 0
        cpu_text_vertices[fv_count] = start_ndc_x; 
        cpu_text_vertices[fv_count+1] = start_ndc_y;
        cpu_text_vertices[fv_count+2] = uv_x;
        cpu_text_vertices[fv_count+3] = uv_y;  

        // box bottom left. 1
        cpu_text_vertices[fv_count+4] = start_ndc_x;
        cpu_text_vertices[fv_count+5] = start_ndc_y-char_height_norm;
        cpu_text_vertices[fv_count+6] = uv_x;
        cpu_text_vertices[fv_count+7] = uv_y+uv_height;

        // box bottom right. 2
        cpu_text_vertices[fv_count+8] = start_ndc_x+char_width_norm;
        cpu_text_vertices[fv_count+9] = start_ndc_y-char_height_norm; 
        cpu_text_vertices[fv_count+10] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+11] = uv_y+uv_height; 


        // box bottom right. 2
        cpu_text_vertices[fv_count+12] = start_ndc_x+char_width_norm;
        cpu_text_vertices[fv_count+13] = start_ndc_y-char_height_norm; 
        cpu_text_vertices[fv_count+14] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+15] = uv_y+uv_height; 

        // box top right. 3 
        cpu_text_vertices[fv_count+16] = start_ndc_x+char_width_norm; 
        cpu_text_vertices[fv_count+17] = start_ndc_y; 
        cpu_text_vertices[fv_count+18] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+19] = uv_y; 

        // top 
        cpu_text_vertices[fv_count+20] = start_ndc_x;
        cpu_text_vertices[fv_count+21] = start_ndc_y;
        cpu_text_vertices[fv_count+22] = uv_x;
        cpu_text_vertices[fv_count+23] = uv_y;

        float xxx = (float)character.originX/SCR_WIDTH;

        start_ndc_x += char_width_norm + xxx;
        fv_count+=24;
        p++;
    } 
    
    gpu_p_mapped_buffer_write(&gpu_pmbuffer_text_vertices, byte_buffer_offset, fv_count*sizeof(float), cpu_text_vertices);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw.
   // assert(r_shader_valid(shader_font));
   // shader_prog_use(shader_font.prog);
    glBindVertexArray(vao_text);
    glBindTexture(GL_TEXTURE_2D, font->texture.id);
    glDrawArrays(GL_TRIANGLES, 0, fv_count);

    glDisable(GL_BLEND);

    fv_count = 0;
}

void console_post_cwd(void)
{
    enum { CWD_PATH_MAX = 1024 };
    char buf[CWD_PATH_MAX];
    char* success = os_get_cwd(buf, sizeof(buf));
    if (success) {fprintf(stderr, "%s\n", buf);} 
    else {fprintf(stderr, "%s\n", "[err: getcwd failed]");}
}

typedef struct {
    Fontek font;
    Texture texture_font;
    Texture texture_grass;
    TextureCubemap texture_skybox;
    GpuMeshIndexed mesh_teapot;
    Camerka camera;
} ProgramContext;

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Light Cubes", NULL, NULL);
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

    // maybe malloc it?
    obj_in_memory teapot_obj = {0};
    if (load_obj_from_path(&teapot_obj, path_teapot) != 0) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", path_teapot);
        abort();
    }
    Fontek arial_font;
    font_create(&arial_font, font_arial_white, program_ctx.texture_font);  

    if (r_renderer_init(&renderer_ctx) != 0) {
        elog_abort("Renderer initialization failed!");
    }



    // prepare ground quad transform
    matf4x4 transform = matf4x4_I_give();
    matf4x4 scale = matf4x4_I_give();
    matf4x4 rotate = matf4x4_I_give();
    matf4x4_scale_set(&scale, 1.0f, 1.0f, 1.0f);
    matf4x4_rot_x(&rotate, deg_to_rad(-90.0f));
    matf4x4 translate = matf4x4_I_give();
    matf4x4 temp = matf4x4_I_give();
    matf4x4_mul(&temp, &rotate, &scale);
    matf4x4_mul(&transform, &translate, &temp);
    shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.model, &transform.col1.x);

    // precalculate planes positionsss.
    const int area_size = 20;
    vecf3 translations[1600];
    int curr = 0;
    for (int z = -area_size; z < area_size; z++) {
        for (int x = -area_size; x < area_size; x++) {
            translations[curr] = (vecf3){(float)x+0.5f, 0.0f, (float)z+0.5f};
            curr++;
        }
    }

    shader_set_3fv(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.offsets, 1600, &translations[0].x); 

    // camera setup PROJECTION SET ONCE AT START !!
    vecf3 pos = (vecf3){-5.0f, 2.0f, 0.0f};
    camerka_set_pos(program_ctx.camera, pos);
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.projection, &projection.col1.x);
    shader_set_mat4(renderer_ctx.shader_skybox.prog, renderer_ctx.shader_skybox.projection, &projection.col1.x);

    gl_enable_depth_test();
    gl_set_clear_color(&(vecf4){0.53f, 0.81f, 0.92f, 1.0f}.x);
    unsigned int frames = 0;
    double delta_time;
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
            log_print_n_flush("[FPS: %d | DELTA TIME: %f]\n", frames, delta_time);
            log_print_n_flush("[CAMERA POS] X: %f, Y: %f, Z: %f\n", program_ctx.camera.pos.x, program_ctx.camera.pos.y, program_ctx.camera.pos.z);
            log_print_n_flush("[CAMERA DIR] X: %f, Y: %f, Z: %f\n", program_ctx.camera.orientation.x, program_ctx.camera.orientation.y, program_ctx.camera.orientation.z);
            frames = 0;
            prev_frame_time = curr_time;
        }

        // HANDLE INPUT
        processInput(window);

        program_ctx.camera.yaw   += mouse_dx * mouse_sens;
        program_ctx.camera.pitch += mouse_dy * mouse_sens;
        if (program_ctx.camera.pitch > 89.0f)  program_ctx.camera.pitch = 89.0f;
        if (program_ctx.camera.pitch < -89.0f) program_ctx.camera.pitch = -89.0f;
        mouse_dx = 0.0; // does platform reset, dunno if here?
        mouse_dy = 0.0;

        vecf3 world_up = (vecf3){0.0f, 1.0f, 0.0f};
        vecf3 movement_vector = VECF3_ZERO;
        vecf3 orient = camerka_orientation(&program_ctx.camera);
        program_ctx.camera.orientation = orient; // cache it.
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
            vecf3_apply_add(&program_ctx.camera.pos, movement_vector);
        }

        // UPDATE THINGS.
        matf4x4 view = camerka_view_matrix(&program_ctx.camera); // you can use shader uniforms that can be shared across multiple shaders.
        shader_set_mat4(renderer_ctx.shader_instanced.prog, renderer_ctx.shader_instanced.view, &view.col1.x);
        shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.view, &view.col1.x);

        matf4x4 view_no_translation = view;
        view_no_translation.col4.x = 0.0f; // zero out translation
        view_no_translation.col4.y = 0.0f;
        view_no_translation.col4.z = 0.0f;
        shader_set_mat4(renderer_ctx.shader_skybox.prog, renderer_ctx.shader_skybox.view, &view_no_translation.col1.x);

        // RENDER START
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // skybox
        glDepthMask(GL_FALSE);
        shader_prog_use(renderer_ctx.shader_skybox.prog);
        glBindVertexArray(renderer_ctx.mesh_skybox.VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, program_ctx.texture_skybox.id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // ground
        shader_prog_use(renderer_ctx.shader_instanced.prog);
        glBindTexture(GL_TEXTURE_2D, program_ctx.texture_grass.id);
        glBindVertexArray(renderer_ctx.mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, renderer_ctx.mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);
       
        // anchor
        shader_prog_use(renderer_ctx.shader_basic_3d.prog);
        matf4x4 s = matf4x4_I_give();
        matf4x4 t = matf4x4_translate_give((vecf3){0.0f, 1.0f, 0.0f});
        matf4x4_scale_set(&s, 0.05f, 0.05f, 0.05f);
        matf4x4 full = matf4x4_I_give();
        matf4x4_mul(&full, &t, &s);
        shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.model, &full.col1.x);
        glBindVertexArray(renderer_ctx.mesh_anchor.VAO);
        glDrawElements(GL_TRIANGLES, renderer_ctx.mesh_anchor.index_count, GL_UNSIGNED_INT, 0);

        // teapot
        static float rotacja = 0.0f;
        shader_prog_use(renderer_ctx.shader_basic_3d.prog);

        matf4x4 s2 = matf4x4_I_give();
        matf4x4 r = matf4x4_I_give();
        matf4x4 t2 = matf4x4_translate_give((vecf3){0.0f, 0.0f, 0.0f});
        matf4x4_scale_set(&s2, 1.0f, 1.0f, 1.0f);
        matf4x4_rot_y(&r, rotacja);
        rotacja += 0.02f;
        matf4x4 full2 = matf4x4_I_give();
        matf4x4 temp = matf4x4_I_give();
        matf4x4_mul(&temp, &r, &s2);
        matf4x4_mul(&full2, &temp, &t2);

        const float teapot_color[] = {1.0f, 1.0f, 0.5f, 1.0f}; 
        shader_set_mat4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.model, &full2.col1.x);
        shader_set_vec4(renderer_ctx.shader_basic_3d.prog, renderer_ctx.shader_basic_3d.color, teapot_color);

        glPointSize(2.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(program_ctx.mesh_teapot.VAO);
        glDrawElements(GL_TRIANGLES, program_ctx.mesh_teapot.index_count, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSwapBuffers(window);
        glfwPollEvents();

        move_w = false;
        move_s = false;
        move_a = false;
        move_d = false;
    }
    
    glfwTerminate();
    return 0;
}
