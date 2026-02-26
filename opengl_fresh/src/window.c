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

// helper for key_callback function.
#define MAP_KEY_RANGE(glfw_key_start, button_start) \
    i32 input_layer_key_index = key-(glfw_key_start)+(button_start); \
    if (action == GLFW_PRESS) { \
        g_input_state.keyboard.buttons_pressed[input_layer_key_index] = 1; \
    } \
    else if (action == GLFW_RELEASE) { \
        g_input_state.keyboard.buttons_released[input_layer_key_index] = 1; \
    }

#define SET_BUTTON(button_code) \
    if (action == GLFW_PRESS) { \
        g_input_state.keyboard.buttons_pressed[(button_code)] = 1; \
    } \
    else if (action == GLFW_RELEASE) \
    { \
        g_input_state.keyboard.buttons_released[(button_code)] = 1; \
    }

internal void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{ 
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        MAP_KEY_RANGE(GLFW_KEY_A, BUTTON_A)
    }
    else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
        MAP_KEY_RANGE(GLFW_KEY_0, BUTTON_0)
    }
    else if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F25) {
        MAP_KEY_RANGE(GLFW_KEY_F1, BUTTON_F1)
    }
    else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL) {
        MAP_KEY_RANGE(GLFW_KEY_KP_0, BUTTON_KP_0)
    }
    else if (key >= GLFW_KEY_LEFT_SHIFT && key <= GLFW_KEY_RIGHT_SUPER) {
        MAP_KEY_RANGE(GLFW_KEY_LEFT_SHIFT, BUTTON_LEFT_SHIFT)
    }
    else if (key >= GLFW_KEY_CAPS_LOCK && key <= GLFW_KEY_PAUSE) {
        MAP_KEY_RANGE(GLFW_KEY_CAPS_LOCK, BUTTON_CAPS_LOCK)
    }
    else if (key >= GLFW_KEY_ESCAPE && key <= GLFW_KEY_END) {
        MAP_KEY_RANGE(GLFW_KEY_ESCAPE, BUTTON_ESCAPE)
    }

    else if (key == GLFW_KEY_SPACE) {
        SET_BUTTON(BUTTON_SPACE)
    }
    else if (key == GLFW_KEY_APOSTROPHE) {
        SET_BUTTON(BUTTON_APOSTROPHE)
    }
    else if (key == GLFW_KEY_COMMA) {
        SET_BUTTON(BUTTON_COMMA)
    }
    else if (key == GLFW_KEY_MINUS) {
        SET_BUTTON(BUTTON_MINUS)
    }
    else if (key == GLFW_KEY_PERIOD) {
        SET_BUTTON(BUTTON_PERIOD)
    }
    else if (key == GLFW_KEY_SLASH) {
        SET_BUTTON(BUTTON_SLASH)
    }
    else if (key == GLFW_KEY_SEMICOLON) {
        SET_BUTTON(BUTTON_SEMICOLON)
    }
    else if (key == GLFW_KEY_EQUAL) {
        SET_BUTTON(BUTTON_EQUAL)
    }
    else if (key == GLFW_KEY_LEFT_BRACKET) {
        SET_BUTTON(BUTTON_LEFT_BRACKET)
    }
    else if (key == GLFW_KEY_BACKSLASH) {
        SET_BUTTON(BUTTON_BACKSLASH)
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET) {
        SET_BUTTON(BUTTON_RIGHT_BRACKET)
    }
    else if (key == GLFW_KEY_GRAVE_ACCENT) {
        SET_BUTTON(BUTTON_GRAVE_ACCENT)
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

