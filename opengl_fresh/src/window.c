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
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
   //     elog_s("LETTER");
    }

    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
        // elog_s("NUMBERS");
    }

    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F25) {
        // elog_s("EFKI");
    }

    if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL) {
        // elog_s("NUMPAD");
    }

    if (key >= GLFW_KEY_LEFT_SHIFT && key <= GLFW_KEY_RIGHT_SUPER) {
        // elog_s("SHIFTY CONTROLE ITP");
    }

    if (key >= GLFW_KEY_CAPS_LOCK && key <= GLFW_KEY_PAUSE) {
        // elog_s("LOCKI I MISC");
    }

    if (key >= GLFW_KEY_ESCAPE && key <= GLFW_KEY_END) {
        // elog_s("ENTERY, TABY, STRZALKI");
    }

    if (key == GLFW_KEY_SPACE) {

    }

    if (key == GLFW_KEY_APOSTROPHE) {

    }

    if (key == GLFW_KEY_COMMA) {

    }

    if (key == GLFW_KEY_MINUS) {

    }

    if (key == GLFW_KEY_PERIOD) {

    }

    if (key == GLFW_KEY_SLASH) {

    }

    if (key == GLFW_KEY_SEMICOLON) {

    }

    if (key == GLFW_KEY_EQUAL) {

    }

    if (key == GLFW_KEY_LEFT_BRACKET) {

    }

    if (key == GLFW_KEY_BACKSLASH) {

    }

    if (key == GLFW_KEY_RIGHT_BRACKET) {

    }

    if (key == GLFW_KEY_GRAVE_ACCENT) {

    }
   
}

void platform_disable_cursor(void)
{
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void platform_normal_cursor(void)
{
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

