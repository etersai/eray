#ifndef ERAY_CANVAS_H_
#define ERAY_CANVAS_H_

#include "eter_math.h"
#include <stdint.h>

typedef struct {
    uint32_t* data; 
    ivec2 origin;    // {CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2} => [320, 180]
    int width;
    int height;
} eCanvas;

int canvas_initialize(eCanvas* canvas, int width, int height);
void canvas_fill(eCanvas* canvas, uint32_t color);
void canvas_put_pixel(eCanvas* canvas, ivec2 pos, uint32_t color); 
void canvas_destroy(eCanvas* canvas);

#endif /* ERAY_CANVAS_H_ */
