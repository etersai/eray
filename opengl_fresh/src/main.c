#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "camerka.h"
#include "platform.h"
#include "lamath.h"
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
//////////////////////////////////////////

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
    GLuint id; 
    int width;
    int height;
    int num_color_channels;
} Texture;

typedef struct {
    GLuint VAO; 
    GLuint VBO; 
    GLsizei vertex_count;
} GpuMeshSimple;

typedef struct {
    GLuint VAO; 
    GLuint VBO; 
    GLuint EBO;
    GLsizei index_count;
} GpuMeshIndexed;

typedef struct {
    ShaderInstanced instance_shader;
    GpuMeshIndexed mesh_quad;
} VoxelRenderer;

GpuMeshIndexed gpu_load_mesh_quad(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size);
GpuMeshIndexed gpu_load_mesh_anchor(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size);
GpuMeshSimple gpu_load_mesh_triangle(const float* vertices, size_t vertices_size);
Texture texture_create_from_memory(unsigned char* data, int width, int height, int color_channels);

Texture texture_load_from_path(const char* path);

// globals
const float mouse_sens = 0.1f;
VoxelRenderer vox;
ShaderBasic shader_basic;
Texture texture_grass;
GpuMeshIndexed mesh_anchor;
Camerka camera;

int main(void)
{
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

    glfwSwapInterval(1); // 1 VSYNC ON / 0 OFF
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
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
    
    shader_basic.program = shader_create_from_memory(vertex_shader_basic_src, fragment_shader_basic_src);
    if (shader_basic.program.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    shader_basic.model = shader_get_uniform_location(shader_basic.program, "model");
    shader_basic.view = shader_get_uniform_location(shader_basic.program, "view");
    shader_basic.projection = shader_get_uniform_location(shader_basic.program, "projection");

    vox.instance_shader.program = shader_create_from_memory(vertex_shader_instance_src, fragment_shader_src); 
    if (vox.instance_shader.program.id == 0) {
        log_print_n_flush("SHADER COMPILATION FAILED!\n");
        abort();
    }
    vox.instance_shader.model = shader_get_uniform_location(vox.instance_shader.program, "model");
    vox.instance_shader.view = shader_get_uniform_location(vox.instance_shader.program, "view");
    vox.instance_shader.projection = shader_get_uniform_location(vox.instance_shader.program, "projection");
    vox.instance_shader.offsets = shader_get_uniform_location(vox.instance_shader.program, "offsets");

    vox.mesh_quad = gpu_load_mesh_quad(quad_verts, quad_indices, sizeof(quad_verts), sizeof(quad_indices));
    mesh_anchor = gpu_load_mesh_anchor(anchor_vertices, anchor_indices, sizeof(anchor_vertices), sizeof(anchor_indices));

    // camera setup
    camera.pos = (vecf3){0.0f, 10.0f, 0.0f};
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(vox.instance_shader.program, vox.instance_shader.projection, &projection.col1.x);
    shader_set_mat4(shader_basic.program, shader_basic.projection, &projection.col1.x);

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
    shader_set_mat4(vox.instance_shader.program, vox.instance_shader.model, &transform.col1.x);

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
    glUseProgram(vox.instance_shader.program.id);
    glUniform3fv(vox.instance_shader.offsets, 1600, &translations[0].x);

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

        // frames and delta time.
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

        // handle input.
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
        // hacky
        orient = vecf3_scale(0.1f, orient);
        if (move_w) {
            vecf3_apply_add(&movement_vector, orient);
            //camera.pos = vecf3_add(camera.pos, orient);
        }
        if (move_s) {
            camera.pos = vecf3_sub(camera.pos, orient);
        }
        if (move_a || move_d) {
            vecf3 right = vecf3_cross(orient, world_up);
            right = vecf3_norm(right);
            right = vecf3_scale(0.1f, right);
            if (move_a) { // scale to lenght off orient ???
                right.x = -right.x;
                right.y = -right.y;
                right.z = -right.z;
            }
            camera.pos = vecf3_add(camera.pos, right);
        }

        // update stuff.
        matf4x4 view = camerka_view_matrix(&camera); // remenber you can use shader uniforms that can be shadred across multiple shaders.
        shader_set_mat4(vox.instance_shader.program, vox.instance_shader.view, &view.col1.x);
        shader_set_mat4(shader_basic.program, shader_basic.view, &view.col1.x);

        // render shit.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(vox.instance_shader.program.id);
        glBindTexture(GL_TEXTURE_2D, texture_grass.id);
        glBindVertexArray(vox.mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, vox.mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);
        //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 400);  
        //glDrawElements(GL_TRIANGLES, mesh_quad.index_count, GL_UNSIGNED_INT, 0);
        
        // gl use program here also.
        glUseProgram(shader_basic.program.id);
        matf4x4 i = matf4x4_I_give();
        matf4x4_scale_set(&i, 0.05f, 0.05f, 0.05f);
        shader_set_mat4(shader_basic.program, shader_basic.model, &i.col1.x);
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

GpuMeshIndexed gpu_load_mesh_anchor(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size)
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // this is unnecesary but for now i leave it.
    glBindVertexArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return (GpuMeshIndexed){VAO, VBO, EBO, indices_size/sizeof(unsigned int)};
}

GpuMeshIndexed gpu_load_mesh_quad(const float* vertices, const unsigned int* indices, size_t vertices_size, size_t indices_size)
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // this is unnecesary but for now i leave it.
    glBindVertexArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return (GpuMeshIndexed){VAO, VBO, EBO, indices_size/sizeof(unsigned int)};
}

GpuMeshSimple gpu_load_mesh_triangle(const float* vertices, size_t vertices_size)
{
    GLuint VBO;
    GLuint VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as 
    // the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally
    // don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    
    return (GpuMeshSimple){VAO, VBO, vertices_size/(sizeof(float)*3)};
}

Texture texture_create_from_memory(unsigned char* data, int width, int height, int color_channels)
{
    assert(data);
    assert(color_channels == 3 && "Texture loading ony supports 3 channgels for now!");

    Texture texture = {0};

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // (S, T = X, Y = U, V)	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Generate gl texutre with picked options.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);       

    return texture;
}


Texture texture_load_from_path(const char* path)
{
    Texture texture = {0};
    int width;
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
