#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { float x; float y; float z; } vecf3;
typedef struct { float data[4][4]; } matf4x4;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

static void matf4x4_print(const matf4x4* matrix);
static void vecf3_print(vecf3 vec);
static inline void matf4x4_identity(matf4x4* matrix);
static inline void matf4x4_test(matf4x4* matrix);
static inline void matf4x4_multiply(const matf4x4* matrix_a, const matf4x4* matrix_b, matf4x4* result);
static inline void matf4x4_translate(matf4x4* matrix, float x, float y, float z);
static inline void matf4x4_translate_set(matf4x4* matrix, float x, float y, float z);
static inline void matf4x4_scale_uniform(matf4x4* matrix, float scale);
static inline void matf4x4_scale(matf4x4* matrix, float sx, float sy, float sz);
static inline void matf4x4_transpose_to_new(const matf4x4* matrix, matf4x4* result);
static inline void matf4x4_transpose_in_place(matf4x4* matrix);
static inline vecf3 transformPoint(const vecf3* point, const matf4x4* matrix);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertex_shader_src = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 scale;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = scale * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragment_shader_src = "#version 460 core\n"
    "out vec4 final_color;\n"
    "void main()\n"
    "{\n"
    "   final_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

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

    // PREPARE TRIANGLE
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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


    // MY STUFF
    matf4x4 scale;
    matf4x4_identity(&scale);
    matf4x4_translate_set(&scale, 1.0f, 0.0f, 0.0f);
    matf4x4_scale_uniform(&scale, 1.0f);
    matf4x4_print(&scale);
    
    glUseProgram(shader_program);
    GLint matrix_loc = glGetUniformLocation(shader_program, "scale");
    glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, &scale.data[0][0]);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
// MY RETARDED MATH
#define MATRIX4X4_PRINT_FORMAT "[%.2f, %.2f, %.2f, %.2f]\n"
#define VECF3_PRINT_FORMAT "[%.2f, %.2f, %.2f]\n"
static void matf4x4_print(const matf4x4* matrix)
{
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[0][0], matrix->data[0][1], matrix->data[0][2], matrix->data[0][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[1][0], matrix->data[1][1], matrix->data[1][2], matrix->data[1][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[2][0], matrix->data[2][1], matrix->data[2][2], matrix->data[2][3]);
    fprintf(stdout, MATRIX4X4_PRINT_FORMAT, matrix->data[3][0], matrix->data[3][1], matrix->data[3][2], matrix->data[3][3]);
    fflush(stdout);
}

static void vecf3_print(vecf3 vec)
{
    fprintf(stdout, VECF3_PRINT_FORMAT, vec.x, vec.y, vec.z);
    fflush(stdout);
}

static inline void matf4x4_identity(matf4x4* matrix)
{
    matrix->data[0][0] = 1.0f;
    matrix->data[0][1] = 0.0f;
    matrix->data[0][2] = 0.0f;
    matrix->data[0][3] = 0.0f;
    
    matrix->data[1][0] = 0.0f;
    matrix->data[1][1] = 1.0f;
    matrix->data[1][2] = 0.0f;
    matrix->data[1][3] = 0.0f; 

    matrix->data[2][0] = 0.0f;
    matrix->data[2][1] = 0.0f;
    matrix->data[2][2] = 1.0f;
    matrix->data[2][3] = 0.0f;
    
    matrix->data[3][0] = 0.0f;
    matrix->data[3][1] = 0.0f;
    matrix->data[3][2] = 0.0f;
    matrix->data[3][3] = 1.0f;
}

static inline void matf4x4_test(matf4x4* matrix)
{
    matrix->data[0][0] = 1.0f;
    matrix->data[0][1] = 2.0f;
    matrix->data[0][2] = 3.0f;
    matrix->data[0][3] = 4.0f;
    
    matrix->data[1][0] = 5.0f;
    matrix->data[1][1] = 6.0f;
    matrix->data[1][2] = 7.0f;
    matrix->data[1][3] = 8.0f; 

    matrix->data[2][0] = 9.0f;
    matrix->data[2][1] = 10.0f;
    matrix->data[2][2] = 11.0f;
    matrix->data[2][3] = 12.0f;
    
    matrix->data[3][0] = 13.0f;
    matrix->data[3][1] = 14.0f;
    matrix->data[3][2] = 15.0f;
    matrix->data[3][3] = 16.0f;
}

static inline void matf4x4_multiply(const matf4x4* matrix_a, const matf4x4* matrix_b, matf4x4* result)
{

}

static inline void matf4x4_translate(matf4x4* matrix, float x, float y, float z)
{
    matrix->data[3][0] += x; 
    matrix->data[3][1] += y; 
    matrix->data[3][2] += z; 
}

static inline void matf4x4_translate_set(matf4x4* matrix, float x, float y, float z)
{
    matrix->data[3][0] = x; 
    matrix->data[3][1] = y; 
    matrix->data[3][2] = z; 
}

static inline void matf4x4_scale_uniform(matf4x4* matrix, float scale)
{
    matrix->data[0][0] *= scale;
    matrix->data[1][1] *= scale;
    matrix->data[2][2] *= scale;
}

static inline void matf4x4_scale(matf4x4* matrix, float sx, float sy, float sz)
{
    matrix->data[0][0] *= sx;
    matrix->data[1][1] *= sy;
    matrix->data[2][2] *= sz;
}

static inline void matf4x4_transpose_to_new(const matf4x4* matrix, matf4x4* result)
{
    // first row to column
    result->data[0][0] = matrix->data[0][0];
    result->data[1][0] = matrix->data[0][1];
    result->data[2][0] = matrix->data[0][2];
    result->data[3][0] = matrix->data[0][3];
    
    // second row to second column
    result->data[0][1] = matrix->data[1][0];
    result->data[1][1] = matrix->data[1][1];
    result->data[2][1] = matrix->data[1][2];
    result->data[3][1] = matrix->data[1][3];

    // third row to third column
    result->data[0][2] = matrix->data[2][0];
    result->data[1][2] = matrix->data[2][1];
    result->data[2][2] = matrix->data[2][2];
    result->data[3][2] = matrix->data[2][3];

    // fourth row to fourth column
    result->data[0][3] = matrix->data[3][0];
    result->data[1][3] = matrix->data[3][1];
    result->data[2][3] = matrix->data[3][2];
    result->data[3][3] = matrix->data[3][3];
}


static inline void matf4x4_transpose_in_place(matf4x4* matrix)
{
    matf4x4 temp;
    
    // first row to column
    temp.data[0][0] = matrix->data[0][0];
    temp.data[1][0] = matrix->data[0][1];
    temp.data[2][0] = matrix->data[0][2];
    temp.data[3][0] = matrix->data[0][3];
    
    // second row to second column
    temp.data[0][1] = matrix->data[1][0];
    temp.data[1][1] = matrix->data[1][1];
    temp.data[2][1] = matrix->data[1][2];
    temp.data[3][1] = matrix->data[1][3];

    // third row to third column
    temp.data[0][2] = matrix->data[2][0];
    temp.data[1][2] = matrix->data[2][1];
    temp.data[2][2] = matrix->data[2][2];
    temp.data[3][2] = matrix->data[2][3];

    // fourth row to fourth column
    temp.data[0][3] = matrix->data[3][0];
    temp.data[1][3] = matrix->data[3][1];
    temp.data[2][3] = matrix->data[3][2];
    temp.data[3][3] = matrix->data[3][3];

    *matrix = temp;
}


static inline vecf3 transformPoint(const vecf3* point, const matf4x4* matrix)
{
    float x = point->x * matrix->data[0][0] + point->y * matrix->data[1][0] + 
              point->z * matrix->data[2][0] + matrix->data[3][0]; 
    float y = point->x * matrix->data[0][1] + point->y * matrix->data[1][1] + 
              point->z * matrix->data[2][1] + matrix->data[3][1]; 
    float z = point->x * matrix->data[0][2] + point->y * matrix->data[1][2] + 
              point->z * matrix->data[2][2] + matrix->data[3][2]; 
    return (vecf3){x, y, z};
}
