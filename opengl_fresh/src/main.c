#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "lamath.h"
#include "shaders.c"

/////////////////////////////////////////////
// THIS GOES TO PLATFORM
const unsigned int SCR_WIDTH = 1280; // 16:9
const unsigned int SCR_HEIGHT = 720; 
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
//////////////////////////////////////////

typedef struct {
    GLuint VBO; 
    GLuint VAO; 
} gpu_mesh; 

gpu_mesh gpu_load_mesh(const float* vertices, size_t size);
GLuint create_program(const char* vertex_shader_src, const char* fragment_shader_src);
void shader_set_model(const matf4x4* model);
void shader_set_view(const matf4x4* view);
void shader_set_projection(const matf4x4* projection);

GLuint shader_program;
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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }    

    float triangle_verts[] = { 
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    float quad_verts[] = { // USE GL_TRIANGLE_STRIP
       -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // bottom-left
        0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // bottom-right
       -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,  // top-left
        0.5f,  0.5f, 0.0f,    1.0f, 1.0f   // top-right
    };

    gpu_mesh mesh_triangle = gpu_load_mesh(triangle_verts, sizeof(triangle_verts)); 
    gpu_mesh mesh_quad     = gpu_load_mesh(quad_verts, sizeof(quad_verts));
    shader_program = create_program(vertex_shader_src, fragment_shader_src);

    // camera setup
    matf4x4 projection;
    float camera_fov = 75.0f;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    lamath_projection_matrix(&projection, camera_fov, aspect, 0.1f, 100.0);
    shader_set_projection(&projection);
    matf4x4 view;
    matf4x4_I(&view);
    lamath_lookat_matrix(&view, (vecf3){0.0f, 0.0f, 0.0f}, (vecf3){0.0f, 0.0f, -1.0f}, (vecf3){0.0f, 1.0f, 0.0f});
    shader_set_view(&view);

    // prepare cube transform
    matf4x4 transform = matf4x4_I_give();
    matf4x4 scale = matf4x4_I_give();
    matf4x4 rotate = matf4x4_I_give();
    matf4x4_scale_set(&scale, 5.0f, 5.0f, 1.0f);
    matf4x4_rot_x(&rotate, deg_to_rad(-45.0f));
    matf4x4 translate = matf4x4_translate_give((vecf3){0.0f, 0.0f, -5.0f}); 
    matf4x4 temp = matf4x4_I_give();
    matf4x4_mul(&temp, &rotate, &scale);
    matf4x4_mul(&transform, &translate, &temp);
    shader_set_model(&transform);



    glUseProgram(shader_program);
    glClearColor(0.32f, 0.32f, 0.32f, 1.0f);
    float rotacja = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        rotacja += 0.01f;

        glClear(GL_COLOR_BUFFER_BIT);
          
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
   // optional 
   // glDeleteVertexArrays(1, &VAO);
   // glDeleteBuffers(1, &VBO);
   // glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

gpu_mesh gpu_load_mesh(const float* vertices, size_t size)
{
    GLuint VBO;
    GLuint VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
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
    
    return (gpu_mesh){VBO, VAO};
}


GLuint create_program(const char* vertex_shader_src, const char* fragment_shader_src)
{
    // PREPARE PROGRAM
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return shader_program;
}

void shader_set_model(const matf4x4* model)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &model->col1.x);
}

void shader_set_projection(const matf4x4* projection)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &projection->col1.x);
}

void shader_set_view(const matf4x4* view)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &view->col1.x);
}

    
