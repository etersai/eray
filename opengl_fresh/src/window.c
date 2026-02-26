#include "window.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "common/types.h"

global GLFWwindow* g_window;
global bool first_mouse = true;

internal void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

