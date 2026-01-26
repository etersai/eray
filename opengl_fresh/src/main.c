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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
        return;
    }

    mouse_dx += xpos - last_x;
    mouse_dy += last_y - ypos; // inverse here.

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
    GLuint id; 
    int width;
    int height;
    int color_channels;
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
GpuMeshSimple gpu_load_mesh_triangle(const float* vertices, size_t vertices_size);
Texture texture_create_from_memory(unsigned char* data, int width, int height, int color_channels);

// globals
const float mouse_sens = 0.1f;
VoxelRenderer vox;
Texture texture_grass;
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

    stbi_set_flip_vertically_on_load(true);  
    const char* asset_grass_texture_path = "assets/grass.jpg";
    int width;
    int height;
    int nrChannels;
    unsigned char *data = stbi_load(asset_grass_texture_path, &width, &height, &nrChannels, 0);
    if (data) {
        log_print_prefix("asset_load_success", "'%s' Width: %d, Height: %d, nrChannels: %d\n",
                 asset_grass_texture_path, width, height, nrChannels);
        texture_grass = texture_create_from_memory(data, width, height, nrChannels);
    }
    else
    {
        // TODO: create temp texture for any failed to load texture [Black, pruple checkerboard pattern :DDD]
        log_print_prefix("asset_load_error", "failed to load '%s'\n", asset_grass_texture_path);
        abort(); // abort for now
    }
    stbi_image_free(data);

    vox.instance_shader.program = shader_create_from_memory(vertex_shader_instance_src, fragment_shader_src); 
    vox.instance_shader.model = shader_get_uniform_location(vox.instance_shader.program, "model");
    vox.instance_shader.view = shader_get_uniform_location(vox.instance_shader.program, "view");
    vox.instance_shader.projection = shader_get_uniform_location(vox.instance_shader.program, "projection");
    vox.instance_shader.offsets = shader_get_uniform_location(vox.instance_shader.program, "offsets");
    vox.mesh_quad = gpu_load_mesh_quad(quad_verts, quad_indices, sizeof(quad_verts), sizeof(quad_indices));

    // camera setup
    matf4x4 projection;
    float camera_fov = 90.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_mat4(vox.instance_shader.program, vox.instance_shader.projection, &projection.col1.x);

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

    //glEnable(GL_CULL_FACE);
   // glCullFace(GL_BACK);
   // glFrontFace(GL_CCW);
   //
//glEnable(GL_CULL_FACE);
//glCullFace(GL_FRONT); 
//
    camera.pos = (vecf3){0.0f, 10.0f, 0.0f};

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

        vecf3 move_vec = VECF3_ZERO;
        vecf3 camerka_forward = camerka_direction(&camera);
        camerka_forward = vecf3_norm(camerka_forward);
        bool moved = false;
        if (move_w) { 
            move_vec = vecf3_add(move_vec, camerka_forward);
            moved = true;
        }
        if (move_s) { 
            dir = vecf3_add(camera.pos, camerka_forward);
            moved = true;
        }
        if (move_a) { 
            dir = vecf3_add(camera.pos, camerka_forward);
            moved = true;
        }
        if (move_d) {
            dir = vecf3_add(camera.pos, camerka_forward);
            moved = true;
        }

        vecf3_add(move_vec, camerka_forward);
        dir = vecf3_add(camera.pos, camerka_forward);

        dir = vecf3_norm(dir);

        
        if (moved) {
            camera.pos = vecf3_norm(vecf3_add(camera.pos, dir));
        } 

        // update stuff.
        matf4x4 view = camerka_view_matrix(&camera); // remenber you can use shader uniforms that can be shadred across multiple shaders.
        shader_set_mat4(vox.instance_shader.program, vox.instance_shader.view, &view.col1.x);

        // render shit.
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture_grass.id);
        glBindVertexArray(vox.mesh_quad.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, vox.mesh_quad.index_count, GL_UNSIGNED_INT, 0, 1600);
        //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 400);  
        //glDrawElements(GL_TRIANGLES, mesh_quad.index_count, GL_UNSIGNED_INT, 0);

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
