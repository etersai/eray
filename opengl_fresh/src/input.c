#include "input.h"

InputState g_input_state = {0};

void input_process(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        
        if (g_input_state.keyboard.buttons_pressed[i] && !g_input_state.keyboard.buttons_released[i]) {
            g_input_state.keyboard.buttons_held[i] = 1;
        }

    }    
}

void input_process_end(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {

        if (g_input_state.keyboard.buttons_released[i]) {
            g_input_state.keyboard.buttons_held[i] = 0;
        }

        g_input_state.keyboard.buttons_pressed[i] = 0;
        g_input_state.keyboard.buttons_released[i] = 0;

    } 
    g_input_state.mouse.dx = 0.0f;
    g_input_state.mouse.dy = 0.0f;
}

bool input_kb_is_button_held(button_type button)
{
    return g_input_state.keyboard.buttons_held[button]; 
}

bool input_kb_is_button_pressed(button_type button)
{
    return g_input_state.keyboard.buttons_pressed[button];
}

bool input_kb_is_button_released(button_type button)
{
    return g_input_state.keyboard.buttons_released[button];
}
