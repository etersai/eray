#ifndef WINDOW_H_
#define WINDOW_H_

#include "common/types.h"

extern u32 window_width;
extern u32 window_height;

b32 open_window_and_setup_gl_context(const char* name, i32 window_width, i32 window_height);
void platform_normal_cursor(void);
void platform_disable_cursor(void);

#endif /* WINDOW_H_ */
