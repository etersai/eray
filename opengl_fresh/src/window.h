#ifndef WINDOW_H_
#define WINDOW_H_

#include "common/types.h"

extern u32 g_window_width;
extern u32 g_window_height;

// KINDA MEH, need another rewrite...
b32 window_open_and_setup_gl_context(const char* name, i32 window_width, i32 window_height);
void window_swap_buffers_and_poll_events(void);
void window_go_fullscreen(void);
void platform_terminate(void);

f64 platform_get_time(void);
void platform_normal_cursor(void);
void platform_disable_cursor(void);

#endif /* WINDOW_H_ */
