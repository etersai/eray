#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "la_math.c"
#include "camerka.c"
#include "shaders.c"

const unsigned int SCR_WIDTH = 1280; // 16:9
const unsigned int SCR_HEIGHT = 720; 

#define LOG_MATRIX(matrix) do { matf4x4_print(&(matrix)); } while(0)
#define LOG_VEC(vector) do { vecf3_print((vector)); } while(0)

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLuint load_triangle(const float* vertices, size_t size);
GLuint create_program(const char* vertex_shader_src, const char* fragment_shader_src);
void shader_set_transform(const matf4x4* transform);
void shader_set_view(const matf4x4* view);
void shader_set_projection(const matf4x4* projection);

// HIGH LEVEL MATH
void make_projection_matrix(const float angleOfView, const float aspect, const float near, const float far, matf4x4* matrix)
{ 
    matf4x4 proj;
    matf4x4_identity(&proj);
    
    float scale = 1.0f / tanf(angleOfView * 0.5f * M_PI / 180.0f); 
    
    proj.data[0][0] = scale / aspect;  // scale x with aspect ratio
    proj.data[1][1] = scale;           // scale y
    proj.data[2][2] = -far / (far - near);
    proj.data[2][3] = -1.0f;           // set w = -z
    proj.data[3][2] = -far * near / (far - near);
    proj.data[3][3] = 0.0f;

    *matrix = proj;
}

void make_lookat_matrix(vecf3 from, vecf3 to, vecf3 arbitraryUp, matf4x4* matrix)
{

}

GLuint  shader_program;
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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }    

    float vertices[] = { 
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    GLuint vao_triangle;
    shader_program = create_program(vertex_shader_src, fragment_shader_src);
    vao_triangle = load_triangle(vertices, sizeof(vertices));

    matf4x4 projection;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    make_projection_matrix(90.0f, aspect, 0.1f, 100.0f, &projection);
    shader_set_projection(&projection);

    camerka_update_pos(&camera, (vecf3){-4.0f, 15.0f, 2.0f});
    camerka_update_aim(&camera, (vecf3){0.0f, -2.0f, -4.0f});

    vecf3 camera_up = {0.0f, 1.0f, 0.0f};
    vecf3 forward = vecf3_norm(vecf3_sub(camera.pos, camera.target));
    LOG_VEC(forward);
    vecf3 right = vecf3_norm(vecf3_cross(forward, camera_up));
    LOG_VEC(right);
    vecf3 up = vecf3_cross(forward, right);
    LOG_VEC(up);

    assert(fabs(vecf3_dot(right, up)) < 1e-5);
    assert(fabs(vecf3_dot(right, forward)) < 1e-5);
    assert(fabs(vecf3_dot(up, forward)) < 1e-5);

    matf4x4 rotation;
    matf4x4_identity(&rotation);
    
    rotation.data[0][0] = right.x;
    rotation.data[1][0] = right.y;
    rotation.data[2][0] = right.z;
    rotation.data[3][0] = -vecf3_dot(right, camera.pos); 

    rotation.data[0][1] = up.x;
    rotation.data[1][1] = up.y;
    rotation.data[2][1] = up.z;
    rotation.data[3][1] = -vecf3_dot(up, camera.pos); 

    rotation.data[0][2] = forward.x;
    rotation.data[1][2] = forward.y;
    rotation.data[2][2] = forward.z;
    rotation.data[3][2] = -vecf3_dot(forward, camera.pos); 

    shader_set_view(&rotation);

    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        glClearColor(0.5f, 1.0f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        matf4x4 left;
        matf4x4_identity(&left);
        matf4x4_translate_set(&left, 0.0f, 0.0f, -1.0f); // calculate on paper location of new pointss
        shader_set_transform(&left);
        glBindVertexArray(vao_triangle); 
        glDrawArrays(GL_TRIANGLES, 0, 3);
           
        matf4x4 right;
        matf4x4_identity(&right);
        matf4x4_translate_set(&right, 0.0f, -2.0f, -4.0f);
        shader_set_transform(&right);
        glBindVertexArray(vao_triangle);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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



GLuint load_triangle(const float* vertices, size_t size)
{   
    unsigned int VBO, VAO;
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
        
    return VAO;
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

void shader_set_transform(const matf4x4* transform)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &transform->data[0][0]);
}

void shader_set_projection(const matf4x4* projection)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &projection->data[0][0]);
}

void shader_set_view(const matf4x4* view)
{
    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &view->data[0][0]);
}

    
