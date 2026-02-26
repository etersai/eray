#include "window.h"
#include "r_opengl.h"
#include "input.h"
#include "common/types.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>

global GLFWwindow* glfw_window;
global bool mouse_first = true;
global f64 mouse_last_x;
global f64 mouse_last_y;

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height) { gl_set_viewport(0, 0, width, height); }

//#define MOUSE_FLIP_Y
internal void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouse_first) {
        mouse_last_x = xpos;
        mouse_last_y = ypos;
        mouse_first = false;
        return;
    }

    g_input_state.mouse.x_pos = xpos;
    g_input_state.mouse.y_pos = ypos;

    g_input_state.mouse.dx += xpos - mouse_last_x;
#ifdef MOUSE_FLIP_Y
    g_input_state.mouse.dy += ypos - mouse_last_y;
#else
    g_input_state.mouse.dy += mouse_last_y - ypos;
#endif /* MOUSE_FLIP_Y */

    mouse_last_x = xpos;
    mouse_last_y = ypos;
}

internal void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{ 
    
}

void platform_disable_cursor(void)
{
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void platform_normal_cursor(void)
{
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

