#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "lamath.h"
#include "camerka.c"
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

#define LOG_MATRIX(matrix) do { matf4x4_print(&(matrix)); } while(0)
#define LOG_VEC(vector) do { vecf3_print((vector)); } while(0)
//////////////////////////////////////////

typedef unsigned int cpu_side_id;

typedef struct {
    GLuint VBO; 
    GLuint VAO; 
} video_card_data; // gpu_mesh ??

typedef struct {
    matf4x4 transform;
    cpu_side_id id;
} quad;

typedef struct {
    matf4x4 transform;
    cpu_side_id id;
} triangle;

#define MAX_TRIANGLES  1024*512
#define MAX_QUADS 1024*512
triangle cpu_triangles[MAX_TRIANGLES];
quad cpu_quads[MAX_QUADS];
size_t triangle_count;
size_t quad_count;

cpu_side_id id_pool_triangle;// (each time)++
cpu_side_id id_pool_quad;

video_card_data gpu_load_quad(const float* vert, size_t size);
video_card_data gpu_load_triangle(const float* vertices, size_t size);
GLuint create_program(const char* vertex_shader_src, const char* fragment_shader_src);
void shader_set_transform(const matf4x4* transform);
void shader_set_view(const matf4x4* view);
void shader_set_projection(const matf4x4* projection);

// HIGH LEVEL MATH
void make_projection_matrix(matf4x4* m, float fov, float aspect, float near, float far)
{
    float f = 1.0f / tanf(fov * 0.5f * M_PI / 180.0f);

    matf4x4 proj = {0};

    proj.col1.x = f / aspect;
    proj.col2.y = f;
    proj.col3.z = -(far + near) / (far - near);
    proj.col3.w = -1.0f;
    proj.col4.z = -(2.0f * far * near) / (far - near);

    *m = proj;
}

void make_lookat_matrix(matf4x4* lookat, vecf3 eye, vecf3 center, vecf3 up)
{ 
    vecf3 f = vecf3_norm(vecf3_sub(center, eye));
    vecf3 s = vecf3_norm(vecf3_cross(f, up));
    vecf3 t = vecf3_cross(s, f);

    lookat->col1.x = s.x;
    lookat->col1.y = t.x;
    lookat->col1.z = -f.x;
    lookat->col1.w = 0.0f;
    
    lookat->col2.x = s.y; 
    lookat->col2.y = t.y;
    lookat->col2.z = -f.y;
    lookat->col2.w = 0.0f;
    
    lookat->col3.x = s.z;
    lookat->col3.y = t.z;
    lookat->col3.z = -f.z;
    lookat->col3.w = 0.0f;
    
    lookat->col4.x = 0.0f;
    lookat->col4.y = 0.0f;
    lookat->col4.z = 0.0f;
    lookat->col4.w = 1.0f;

    lookat->col4.x = -vecf3_dot(s, eye);
    lookat->col4.y = -vecf3_dot(t, eye);
    lookat->col4.z =  vecf3_dot(f, eye);
    lookat->col4.w = 1.0f;
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

    float tri_vertices[] = { 
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    float quad_verts[] = {
       -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // bottom-left
        0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // bottom-right
       -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,  // top-left
        0.5f,  0.5f, 0.0f,    1.0f, 1.0f   // top-right
    };

    video_card_data video_card_triangle = gpu_load_triangle(tri_vertices, sizeof(tri_vertices));
    video_card_data video_card_quad = gpu_load_quad(quad_verts, sizeof(quad_verts));
    shader_program = create_program(vertex_shader_src, fragment_shader_src);

    
    


    // add triangle
    triangle triangle; 
    // make transform
    matf4x4 scale; 
    matf4x4 translate; 
    matf4x4_scale_one_axis(&scale, 20.0f, AXIS_X);
    matf4x4_translate_make(&translate, (vecf3){5.0f, 0.0f, -20.0f});
    matf4x4_mul(&triangle.transform, &translate, &scale);
    // register
    triangle.id = id_pool_triangle;
    cpu_triangles[triangle_count] = triangle;
    id_pool_triangle++;
    triangle_count++;


    
    



    // camera setup
    matf4x4 projection;
    float aspect = (float)SCR_WIDTH / SCR_HEIGHT;
    make_projection_matrix(&projection, 90.0f, aspect, 0.1f, 100.0);
    shader_set_projection(&projection);

    camerka_update_pos(&camera, (vecf3){-4.0f, 15.0f, 2.0f});
    camerka_update_aim(&camera, (vecf3){0.0f, -2.0f, -4.0f});

    matf4x4 view;
    matf4x4_I(&view);
    make_lookat_matrix(&view, (vecf3){0.0f, 0.0f, -5.0f}, (vecf3){0.0f, 0.0f, 0.0f}, (vecf3){0.0f, 1.0f, 0.0f});
    shader_set_view(&view);





    float rotacja = 0.0f;
    glUseProgram(shader_program);
    while (!glfwWindowShouldClose(window)) {

        processInput(window);
        rotacja += 10.0f;

        glClearColor(0.5f, 1.0f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        matf4x4 lt;
        matf4x4_I(&lt);
        matf4x4 rot_y;
        matf4x4_rot_y(&rot_y, deg_to_rad(rotacja));
        vecf3 t1 = {0.0f, 0.0f, -0.90f};
        matf4x4_translate_set(&lt, t1); // calculate on paper location of new points
        matf4x4 res;
        matf4x4_mul(&res, &lt, &rot_y);
        shader_set_transform(&res);
        glBindVertexArray(vao_triangle); 
        glDrawArrays(GL_TRIANGLES, 0, 3);
           
        matf4x4 rt;
        matf4x4_I(&rt);
        vecf3 t2 = {2.0f, 0.0f, -2.5f};
        matf4x4_translate_set(&rt, t2);
        shader_set_transform(&rt);
        glBindVertexArray(vao_triangle);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        matf4x4 translate_quad;




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


video_card_data gpu_load_quad(const float* vert, size_t size)
{
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vert, GL_STATIC_DRAW);
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
     
}

video_card_data gpu_load_triangle(const float* vertices, size_t size)
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
    glUniformMatrix4fv(loc, 1, GL_FALSE, &transform->col1.x);
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

    
