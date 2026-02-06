#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "platform.h"
#include "gl_stuff.h"
#include "obj_loader.h"
#include "lamath.h"

#include "camerka.h"
#include "shader.h"
#include "glsl_shaders.c"
#include "data_vertex.c"
#include "data_font.h"

// VERY IMPORTANT TODOS //
// TODO: ADD ERROR CHECKS FOR GL STUFF.
// TODO: ADD AND ENABLE GL DEBUG FUNCTIONALITY.
// TODO: PROPER LOGGING FUNCTIONALITY
// TODO: SEPERATE PLATFORM LAYER

/////////////////////////////////////////////
// THIS GOES TO PLATFORM
bool move_w;
bool move_s;
bool move_a;
bool move_d;
const unsigned int SCR_WIDTH = 1280; // 16:9
const unsigned int SCR_HEIGHT = 720; 
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
double last_x;
double last_y;
double mouse_dx;
double mouse_dy;
static bool first_mouse = true;
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

const int cwd_path_max = 1024;
void console_post_cwd(void)
{
    char buf[cwd_path_max];
    char* success = eray_get_cwd(buf, sizeof(buf));
    if (success) {fprintf(stderr, "%s\n", buf);} 
    else {fprintf(stderr, "%s\n", "[err: getcwd failed]");}
}

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
    assert(texture_valid(texture));
    font->metadata = font_metadata;
    font->texture = texture;
    return 0;
}

// Asset loading = CPU side (parsing files, decoding)
// Resource creation = GPU upload (creating textures/buffers)
// Rendering = Draw commands
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
    ShaderBasic basic;
    ShaderBasic teapot;
    ShaderInstanced ground;
    ShaderSkybox skybox;
} RenderekShaders;

typedef struct {
    Texture grass;
    TextureCubemap skybox;
} RenderekTextures;

typedef struct {
    GpuMeshIndexed quad;
    GpuMeshIndexed cube;
    GpuMeshIndexed anchor;
    GpuMeshIndexed model;
    GpuMeshSimple  skybox; // -1 to 1 cube at (0,0,0) [36 vertices, only pos]
} RenderekMeshes;

typedef struct {
    RenderekMeshes   meshes;
    RenderekShaders  shaders;
    RenderekTextures textures;
} Renderek;

#define shader_valid(shader) ((shader).prog.id != 0)

// globals
const float mouse_sens = 0.1f;
ShaderBasic shader_basic;
ShaderBasic shader_basic_teapot;
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

//Renderek g_renderek; // not used atm
Camerka camera;


// DRAW FONT STUFF //
////////////////////
#define TEXT_MAX_BYTES (1024*512) // 0.5kib
// relic of the ancient wisdom.
//static unsigned char cpu_text_vertices[TEXT_MAX_BYTES];
static float cpu_text_vertices[TEXT_MAX_BYTES/sizeof(float)]; // 5461.33 CHARS.                                                //
static GpuPMappedBuffer gpu_pmbuffer_text_vertices;
void r_draw_text_immediate(Fontek* font, float x, float y, const char* text)
{
    assert(font);
    assert(texture_valid(font->texture));

    // kickstart it's self kinda thing.
    if (!gl_buffer_valid(gpu_pmbuffer_text_vertices)) { // if not created

        gpu_pmbuffer_text_vertices = gpu_p_mapped_buffer_create(TEXT_MAX_BYTES);

        if (!gl_buffer_valid(gpu_pmbuffer_text_vertices)) {
            elog_abort("[draw text buffer creation]");
        }
    }
    
    struct uv {
        float x;
        float y;
        float w;
        float h;
    };

    float norm_x = x/SCR_WIDTH;
    float norm_y = y/SCR_HEIGHT;
    float start_ndc_x = norm_x * 2 - 1; // remap
    float start_ndc_y = -(norm_y * 2 - 1); // flip for it to match opengl.

    unsigned int fv_count = 0; 
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

        // one char 24 floats 96 bytes.
        // Generate 2 triangles 
        // box top left. 0
    
        cpu_text_vertices[fv_count] = start_ndc_x; 
        cpu_text_vertices[fv_count+1] = start_ndc_y;
        cpu_text_vertices[fv_count+2] = uv_x;
        cpu_text_vertices[fv_count+3] = uv_y;  

        // box bottom left. 1
        cpu_text_vertices[fv_count+4] = start_ndc_x;
        cpu_text_vertices[fv_count+5] = start_ndc_y-uv_height;
        cpu_text_vertices[fv_count+6] = uv_x;
        cpu_text_vertices[fv_count+7] = uv_y+uv_height;

        // box bottom right. 2
        cpu_text_vertices[fv_count+8] = start_ndc_x+uv_width;
        cpu_text_vertices[fv_count+9] = start_ndc_y-uv_height; 
        cpu_text_vertices[fv_count+10] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+11] = uv_y+uv_height; 


        // box bottom right. 2
        cpu_text_vertices[fv_count+12] = start_ndc_x+uv_width;
        cpu_text_vertices[fv_count+13] = start_ndc_y-uv_height; 
        cpu_text_vertices[fv_count+14] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+15] = uv_y+uv_height; 

        // box top right. 3 
        cpu_text_vertices[fv_count+16] = start_ndc_x+uv_width; 
        cpu_text_vertices[fv_count+17] = start_ndc_y; 
        cpu_text_vertices[fv_count+18] = uv_x+uv_width; 
        cpu_text_vertices[fv_count+19] = uv_y; 

        // top 
        cpu_text_vertices[fv_count+20] = start_ndc_x;
        cpu_text_vertices[fv_count+21] = start_ndc_y;
        cpu_text_vertices[fv_count+22] = uv_x;
        cpu_text_vertices[fv_count+23] = uv_y;

        fv_count+=24;
        p++;
    } 

    for (int i = 0; i < fv_count; i++) {
        elog_f(cpu_text_vertices[i]);
    }
    
    gpu_p_mapped_buffer_write(&gpu_pmbuffer_text_vertices, fv_count*sizeof(float), cpu_text_vertices);
}

int main(void)
{
    // Setup GLFW and OpenGL Context
    console_post_cwd();
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
    
    // load teapot model.
    obj_in_memory teapot_obj;
    if (load_obj_from_path(&teapot_obj, path_teapot) != 0) {
        elog_abort("OBJ LOAD FAIL!");
    }

    vecf3 teapot_centers[6320];
    vecf3 teapot_normals[6320];
    vecf3 teapot_normals_lines[6320*2];
    int centroid_count = 0;

    for (size_t i = 0; i < teapot_obj.i_count; i+=3) {

        vecf3 triangle[3];

        unsigned int t_one_index = teapot_obj.indices[i] * 3; // uncoding
        unsigned int t_two_index = teapot_obj.indices[i+1] * 3;
        unsigned int t_three_index = teapot_obj.indices[i+2] * 3;

        triangle[0] = (vecf3){teapot_obj.vertices[t_one_index],
                              teapot_obj.vertices[t_one_index+1],
                              teapot_obj.vertices[t_one_index+2]};
        triangle[1] = (vecf3){teapot_obj.vertices[t_two_index],
                              teapot_obj.vertices[t_two_index+1],
                              teapot_obj.vertices[t_two_index+2]};
        triangle[2] = (vecf3){teapot_obj.vertices[t_three_index],
                              teapot_obj.vertices[t_three_index+1],
                              teapot_obj.vertices[t_three_index+2]};

        //centroids[centroid_count] = lamath_triangle_centroid(triangle);
        vecf3 center = lamath_triangle_centroid(triangle);
        vecf3 normal = lamath_calc_triangle_normal(triangle); 
    
        // PIECE OF HIGH TEC CODE.
        // float dot = center.x*normal.x + center.y*normal.y + center.z*normal.z;
        // if (dot < 0.0f) {
        //     normal.x = -normal.x;
        //     normal.y = -normal.y;
        //     normal.z = -normal.z;
        // }

        teapot_centers[centroid_count].x = center.x;
        teapot_centers[centroid_count].y = center.y;
        teapot_centers[centroid_count].z = center.z;

        teapot_normals[centroid_count+1].x = normal.x;
        teapot_normals[centroid_count+1].y = normal.y;
        teapot_normals[centroid_count+1].z = normal.z;

        centroid_count++;
    } 

    float tn_scale = 0.1f;
    for (int i = 0; i < 6320; i++) {
        // MERGE TWO BUFFER TOGGERTHER KINDA CODE...
        // Start point of line
        teapot_normals_lines[i*2] = teapot_centers[i];
        // End point of line (center + scaled normal)
        teapot_normals_lines[i*2+1].x = teapot_centers[i].x + (teapot_normals[i].x * tn_scale);
        teapot_normals_lines[i*2+1].y = teapot_centers[i].y + (teapot_normals[i].y * tn_scale);
        teapot_normals_lines[i*2+1].z = teapot_centers[i].z + (teapot_normals[i].z * tn_scale);
    }

    // prepare gpu resources.
    texture_grass = texture_load_from_path(path_grass);
    texture_font_arial_white = texture_load_from_path(path_font); 
    if (!texture_valid(texture_grass)) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", path_grass);
        abort();
    }
    if (!texture_valid(texture_font_arial_white)) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", path_font);
        abort();
    }
    
    // CUBEMAP //
    texture_skybox = texture_cubemap_create_from_paths(paths_cubemap);
    if (texture_skybox.id == 0) {
        // TODO: create temp texture for any failed to load texture [Black, pruple checkerboard pattern :DDD]
        log_print_prefix("asset_load_error", "failed to load cubemap/skybox!\n");
        abort();
    }
     
    Fontek arial_font;
    font_create(&arial_font, font_arial_white, texture_grass);  
    r_draw_text_immediate(&arial_font, 1250.0f, 700.0f, "a");

                    
    // SHADERS //
    // basic shader.  
    shader_basic.prog = shader_prog_create_from_memory(glsl_basic_vs, glsl_basic_fs);
    if (!shader_valid(shader_basic)) {
        elog_abort("ABORT FOR NOW"); 
    }
    shader_basic.model = shader_get_uniform_location(shader_basic.prog, "model");
    shader_basic.view = shader_get_uniform_location(shader_basic.prog, "view");
    shader_basic.projection = shader_get_uniform_location(shader_basic.prog, "projection");

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

    
    // precalculated
    GpuMeshSimple mesh_teapot_normals;
    mesh_teapot_normals = gpu_load_mesh_simple_1attr(&teapot_normals_lines[0].x, sizeof(teapot_normals_lines));


    // camera setup PROJECTION SET ONCE AT START !!
    camera.pos = (vecf3){-5.0f, 2.0f, 0.0f};
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(shader_instanced.prog, shader_instanced.projection, &projection.col1.x);
    shader_set_mat4(shader_basic.prog, shader_basic.projection, &projection.col1.x);
    shader_set_mat4(shader_skybox.prog, shader_skybox.projection, &projection.col1.x);
    shader_set_mat4(shader_basic_teapot.prog, shader_basic_teapot.projection, &projection.col1.x);

    // prepare quad transform
    matf4x4 transform = matf4x4_I_give();
    matf4x4 scale = matf4x4_I_give();
    matf4x4 rotate = matf4x4_I_give();
    matf4x4_scale_set(&scale, 1.0f, 1.0f, 1.0f);
    matf4x4_rot_x(&rotate, deg_to_rad(-90.0f));
    matf4x4 translate = matf4x4_I_give();
    matf4x4 temp = matf4x4_I_give();
    matf4x4_mul(&temp, &rotate, &scale);
    matf4x4_mul(&transform, &translate, &temp);
    shader_set_mat4(shader_instanced.prog, shader_instanced.model, &transform.col1.x);

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
    
    // send to gpu. // hackyy
    glUseProgram(shader_instanced.prog.id);
    glUniform3fv(shader_instanced.offsets, 1600, &translations[0].x);


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
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
            log_print_n_flush("[CAMERA POS] X: %f, Y: %f, Z: %f\n", camera.pos.x, camera.pos.y, camera.pos.z);
            log_print_n_flush("[CAMERA DIR] X: %f, Y: %f, Z: %f\n", camera.orientation.x, camera.orientation.y, camera.orientation.z);
            frames = 0;
            prev_frame_time = curr_time;
        }

        // HANDLE INPUT
        processInput(window);

        camera.yaw   += mouse_dx * mouse_sens;
        camera.pitch += mouse_dy * mouse_sens;
        if (camera.pitch > 89.0f)  camera.pitch = 89.0f;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;
        mouse_dx = 0.0; // does platform reset, dunno if here?
        mouse_dy = 0.0;

        vecf3 world_up = (vecf3){0.0f, 1.0f, 0.0f};
        vecf3 movement_vector = VECF3_ZERO;
        vecf3 orient = camerka_orientation(&camera);
        camera.orientation = orient; // cache it.
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
            vecf3_apply_add(&camera.pos, movement_vector);
        }

        // UPDATE THINGS.
        matf4x4 view = camerka_view_matrix(&camera); // you can use shader uniforms that can be shared across multiple shaders.
        shader_set_mat4(shader_instanced.prog, shader_instanced.view, &view.col1.x);
        shader_set_mat4(shader_basic.prog, shader_basic.view, &view.col1.x);
        shader_set_mat4(shader_basic_teapot.prog, shader_basic_teapot.view, &view.col1.x);

        matf4x4 view_no_translation = view;
        view_no_translation.col4.x = 0.0f; // zero out translation
        view_no_translation.col4.y = 0.0f;
        view_no_translation.col4.z = 0.0f;
        shader_set_mat4(shader_skybox.prog, shader_skybox.view, &view_no_translation.col1.x);

        // RENDER START
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Frustum culling solution:
        // CPU-side test: For each quad, check if its bounding box intersects the view frustum.
        // Only send visible quads to GPU.

        // Depth mask info.
        //Fragments still test against the depth buffer (can still be rejected)
        //But passing fragments don't update the depth buffer
        //The depth values stay whatever they were before

        // skybox
        glDepthMask(GL_FALSE);
        shader_prog_use(shader_skybox.prog);
        glBindVertexArray(mesh_skybox.VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox.id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // ground
        shader_prog_use(shader_instanced.prog);
        glBindTexture(GL_TEXTURE_2D, texture_grass.id);
        glBindVertexArray(mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);
       
        // anchor
        shader_prog_use(shader_basic.prog);
        matf4x4 s = matf4x4_I_give();
        matf4x4 t = matf4x4_translate_give((vecf3){0.0f, 1.0f, 0.0f});
        matf4x4_scale_set(&s, 0.05f, 0.05f, 0.05f);
        matf4x4 full = matf4x4_I_give();
        matf4x4_mul(&full, &t, &s);
        shader_set_mat4(shader_basic.prog, shader_basic.model, &full.col1.x);
        glBindVertexArray(mesh_anchor.VAO);
        glDrawElements(GL_TRIANGLES, mesh_anchor.index_count, GL_UNSIGNED_INT, 0);

        // teapot
        static float rotacja = 0.0f;
        shader_prog_use(shader_basic_teapot.prog);

        matf4x4 s2 = matf4x4_I_give();
        matf4x4 r = matf4x4_I_give();
        matf4x4 t2 = matf4x4_translate_give((vecf3){0.0f, 0.0f, 0.0f});
        matf4x4_scale_set(&s2, 1.0f, 1.0f, 1.0f);
        matf4x4_rot_y(&r, rotacja);
        rotacja += 0.00f;
        matf4x4 full2 = matf4x4_I_give();
        matf4x4 temp = matf4x4_I_give();
        matf4x4_mul(&temp, &r, &s2);
        matf4x4_mul(&full2, &temp, &t2);

        shader_set_mat4(shader_basic_teapot.prog, shader_basic_teapot.model, &full2.col1.x);

        const float teapot_color[] = {1.0f, 0.5f, 0.0f, 1.0f}; 
        shader_set_vec4(shader_basic_teapot.prog, shader_basic_teapot.color, teapot_color);

        glPointSize(2.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(mesh_model.VAO);
        glDrawElements(GL_TRIANGLES, mesh_model.index_count, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // teapot normals.
        shader_prog_use(shader_basic_teapot.prog);
        matf4x4 cm = matf4x4_I_give();
        shader_set_mat4(shader_basic_teapot.prog, shader_basic_teapot.model, &cm.col1.x);
        vecf4 xd =(vecf4){0.0f, 1.0f, 0.0f, 1.0f};
        shader_set_vec4(shader_basic_teapot.prog, shader_basic_teapot.color, &xd.x);
        //glPointSize(5.0f);
        glBindVertexArray(mesh_teapot_normals.VAO);
        glDrawArrays(GL_LINES, 0, mesh_teapot_normals.vertex_count);

        glfwSwapBuffers(window);
        glfwPollEvents();
        move_w = false;
        move_s = false;
        move_a = false;
        move_d = false;
    }
    
   // optional cleanup for now at least. 
   // glDeleteVertexArrays(1, &VAO);
   // glDeleteBuffers(1, &VBO);
   // glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}
