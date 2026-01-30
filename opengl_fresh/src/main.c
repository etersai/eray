#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "platform.h"
#include "lamath.h"
#include "gl_stuff.h"
#include "camerka.h"
#include "shader.h"
#include "font.c"
#include "vertex_data.c"
#include "shaders_src.c"

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

typedef GLint uniform;
typedef struct {
    ShaderProgram program;
    uniform model;
    uniform view;
    uniform projection;
    uniform offsets;
} ShaderInstanced;

typedef struct {
    ShaderProgram program;
    uniform model;
    uniform view;
    uniform projection;
} ShaderBasic;

typedef struct {
    ShaderProgram program;
    uniform view;
    uniform projection;
} ShaderSkybox;

Texture texture_load_from_path(const char* path)
{
    Texture texture = {0};
    int width; // TODO platform extract image data!
    int height;
    int num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);
    if (data) {
        //log_print_prefix("asset_load_success", "'%s' Width: %d, Height: %d, nrChannels: %d\n",
        //        path, texture.width, texture.height, texture.num_color_channels);
        texture = texture_create_from_memory(data, width, height, num_channels);
        stbi_image_free(data);
        return texture;
    }
    return texture;
}

void do_some_shit(char* file, size_t size, float* vertices, unsigned int* indices, size_t* vert_count, size_t* index_count)
{
    enum { MAX_BUF = 64 };
    char line_buffer[MAX_BUF];
    int line_buffer_count = 0;

    char* ptr = file;
    char* end = file + size;
    while (ptr < end) {
#if 1
        if (*ptr == '\n') {

            line_buffer[line_buffer_count] = '\0';

            if (line_buffer[0] == 'v') {
                float temp_buf[3];
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok;
                tok = strtok(ptr_temp, " "); // strtok modifies buffer 
                int k = 0;
                while (tok) {
                    float val;
                    int result = sscanf(tok, "%f", &val);
                    if (result == 0) {
                        abort();
                    }
                    temp_buf[k] = val; 
                    tok = strtok(NULL, " ");
                    k++;
                } 
                elog_f(temp_buf[0]);
                elog_f(temp_buf[1]);
                elog_f(temp_buf[2]);
            }

            else if (line_buffer[0] == 'f') {
                unsigned int temp_buf[3];
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok;
                tok = strtok(ptr_temp, " "); // strtok puts '\0' at delimiters 
                int k = 0;
                while (tok) {
                    unsigned int val;
                    int result = sscanf(tok, "%u", &val);
                    if (result == 0) {
                        abort();
                    }
                    temp_buf[k] = val; 
                    tok = strtok(NULL, " ");
                    k++;
                }
                elog_d(temp_buf[0]);
                elog_d(temp_buf[1]);
                elog_d(temp_buf[2]);
            }
            // go to next line.
            line_buffer_count = 0;
            ptr++;
            continue;
        }
#endif
        line_buffer[line_buffer_count] = *ptr;
        line_buffer_count++;
        assert(line_buffer_count < MAX_BUF);
        putchar(*ptr);
        ptr++;
    }

}

const int cwd_path_max = 1024;
void console_post_cwd(void)
{
    char buf[cwd_path_max];
    char* success = eray_get_cwd(buf, sizeof(buf));
    if (success) {fprintf(stdout, "%s\n", buf);} 
    else {fprintf(stdout, "%s\n", "[err: getcwd failed]");}
}

// globals
const float mouse_sens = 0.1f;
ShaderBasic shader_basic;
ShaderInstanced shader_instanced;
ShaderSkybox shader_skybox;
GpuMeshIndexed mesh_quad;
GpuMeshIndexed mesh_anchor;
GpuMeshSimple  mesh_skybox; // -1 to 1 cube at (0,0,0) [36 vertices, only pos]
Texture texture_grass;
TextureCubemap texture_skybox;
Camerka camera;

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

    glfwSwapInterval(1); // 1 VSYNC ON / 0 OFF

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


    const char* filename = "./assets/models/teapot.obj";
    size_t size;
    char* file = map_file_into_memory(filename, &size);  
    if (file == NULL) {
        log_print_prefix("asset_load_error", "failed to load '%s'\n", filename);
        abort();
    }

    float vertices[1024*1024];// 1 mib
    unsigned int indices[1024*1024];

    enum { MAX_BUF = 64 };
    char line_buffer[MAX_BUF];
    int line_buffer_count = 0;
    char* ptr = file;
    char* end = file + size;
    while (ptr < end) {
#if 1
        if (*ptr == '\n') {
            line_buffer[line_buffer_count] = '\0';
            if (line_buffer[0] == 'v') {
                float temp_buf[3];
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok;
                tok = strtok(ptr_temp, " "); // strtok modifies buffer 
                int k = 0;
                while (tok) {
                    float val;
                    int result = sscanf(tok, "%f", &val);
                    if (result == 0) {
                        abort();
                    }
                    temp_buf[k] = val; 
                    tok = strtok(NULL, " ");
                    k++;
                } 
                elog_f(temp_buf[0]);
                elog_f(temp_buf[1]);
                elog_f(temp_buf[2]);
            }
            else if (line_buffer[0] == 'f') {
                unsigned int temp_buf[3];
                char* ptr_temp = line_buffer;
                ptr_temp+=2; // skip v_ and f_ . _ as whitespace 
                char* tok;
                tok = strtok(ptr_temp, " "); // strtok puts '\0' at delimiters 
                int k = 0;
                while (tok) {
                    unsigned int val;
                    int result = sscanf(tok, "%u", &val);
                    if (result == 0) {
                        abort();
                    }
                    temp_buf[k] = val; 
                    tok = strtok(NULL, " ");
                    k++;
                }
                elog_d(temp_buf[0]);
                elog_d(temp_buf[1]);
                elog_d(temp_buf[2]);
            }
            // go to next line.
            line_buffer_count = 0;
            ptr++;
            continue;
        }
#endif
        line_buffer[line_buffer_count] = *ptr;
        line_buffer_count++;
        assert(line_buffer_count < MAX_BUF);
        putchar(*ptr);
        ptr++;
    }


    // prepare gpu resources.
    stbi_set_flip_vertically_on_load(true);  
    const char* asset_path = "assets/grass.jpg";
    texture_grass = texture_load_from_path(asset_path);
    if (texture_grass.id == 0) {
        // TODO: create temp texture for any failed to load texture [Black, pruple checkerboard pattern :DDD]
        log_print_prefix("asset_load_error", "failed to load '%s'\n", asset_path);
        abort();
    }
    
    // CUBEMAP //
    const char* cubemap_paths[] = {  // realtionship.
        "assets/skybox/right.jpg",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        "assets/skybox/left.jpg",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        "assets/skybox/top.jpg",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        "assets/skybox/bottom.jpg",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        "assets/skybox/front.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        "assets/skybox/back.jpg"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    }; 
    texture_skybox = texture_cubemap_create_from_paths(cubemap_paths);
    if (texture_skybox.id == 0) {
        // TODO: create temp texture for any failed to load texture [Black, pruple checkerboard pattern :DDD]
        log_print_prefix("asset_load_error", "failed to load cubemap/skybox!\n");
        abort();
    }
     

    // SHADERS //
    // basic shader. 
    shader_basic.program = shader_create_from_memory(vertex_shader_basic_src, fragment_shader_basic_src);
    if (shader_basic.program.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    shader_basic.model = shader_get_uniform_location(shader_basic.program, "model");
    shader_basic.view = shader_get_uniform_location(shader_basic.program, "view");
    shader_basic.projection = shader_get_uniform_location(shader_basic.program, "projection");

    // skybox shader.
    shader_skybox.program = shader_create_from_memory(vertex_shader_skybox_src, fragment_shader_skybox_src);
    if (shader_skybox.program.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    shader_skybox.view = shader_get_uniform_location(shader_skybox.program, "view");
    shader_skybox.projection = shader_get_uniform_location(shader_skybox.program, "projection");

    // ground shader.
    shader_instanced.program = shader_create_from_memory(vertex_shader_instance_src, fragment_shader_src); 
    if (shader_instanced.program.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    shader_instanced.model = shader_get_uniform_location(shader_instanced.program, "model");
    shader_instanced.view = shader_get_uniform_location(shader_instanced.program, "view");
    shader_instanced.projection = shader_get_uniform_location(shader_instanced.program, "projection");
    shader_instanced.offsets = shader_get_uniform_location(shader_instanced.program, "offsets");

    // LOAD MESHES
    mesh_quad = gpu_load_mesh_quad(quad_verts, quad_indices, sizeof(quad_verts), sizeof(quad_indices));
    mesh_anchor = gpu_load_mesh_anchor(anchor_vertices, anchor_indices, sizeof(anchor_vertices), sizeof(anchor_indices));
    mesh_skybox = gpu_load_mesh_simple_1attr(skyboxVertices, sizeof(skyboxVertices));

    // camera setup PROJECTION SET ONCE AT START !!
    camera.pos = (vecf3){0.0f, 10.0f, 0.0f};
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(shader_instanced.program, shader_instanced.projection, &projection.col1.x);
    shader_set_mat4(shader_basic.program, shader_basic.projection, &projection.col1.x);
    shader_set_mat4(shader_skybox.program, shader_skybox.projection, &projection.col1.x);

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
    shader_set_mat4(shader_instanced.program, shader_instanced.model, &transform.col1.x);

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
    glUseProgram(shader_instanced.program.id);
    glUniform3fv(shader_instanced.offsets, 1600, &translations[0].x);

    // FISHCARD //
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT); 

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
        shader_set_mat4(shader_instanced.program, shader_instanced.view, &view.col1.x);
        shader_set_mat4(shader_basic.program, shader_basic.view, &view.col1.x);

        matf4x4 view_no_translation = view;
        view_no_translation.col4.x = 0.0f; // zero out translation
        view_no_translation.col4.y = 0.0f;
        view_no_translation.col4.z = 0.0f;
        shader_set_mat4(shader_skybox.program, shader_skybox.view, &view_no_translation.col1.x);

        // RENDER START
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Frustum culling solution:
        // CPU-side test: For each quad, check if its bounding box intersects the view frustum.
        // Only send visible quads to GPU.

        // Depth mask info.
        //Fragments still test against the depth buffer (can still be rejected)
        //But passing fragments don't update the depth buffer
        //The depth values stay whatever they were before
        glDepthMask(GL_FALSE);
        shader_use(shader_skybox.program);
        glBindVertexArray(mesh_skybox.VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox.id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // ground
        shader_use(shader_instanced.program);
        glBindTexture(GL_TEXTURE_2D, texture_grass.id);
        glBindVertexArray(mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);
        //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 400);  
        //glDrawElements(GL_TRIANGLES, mesh_quad.index_count, GL_UNSIGNED_INT, 0);
       
        // anchor
        shader_use(shader_basic.program);
        matf4x4 s = matf4x4_I_give();
        matf4x4 t = matf4x4_translate_give((vecf3){0.0f, 1.0f, 0.0f});
        matf4x4_scale_set(&s, 0.05f, 0.05f, 0.05f);
        matf4x4 full = matf4x4_I_give();
        matf4x4_mul(&full, &t, &s);
        shader_set_mat4(shader_basic.program, shader_basic.model, &full.col1.x);
        glBindVertexArray(mesh_anchor.VAO);
        glDrawElements(GL_TRIANGLES, mesh_anchor.index_count, GL_UNSIGNED_INT, 0);

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
