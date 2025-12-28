#include "eray_canvas.h"

#include <stdlib.h>
#include <assert.h>

int canvas_initialize(eCanvas* canvas, int width, int height)
{
    assert(canvas && "[TODO]");
    uint32_t* data;
    data = malloc(width*height*sizeof(*data));
    if (!data) return 1;

    canvas->data   = data;
    canvas->width  = width;
    canvas->height = height;

    return 0;
}

void canvas_fill(eCanvas* canvas, uint32_t color)
{
    size_t pixel_count = canvas->width*canvas->height;
    for (size_t i = 0; i < pixel_count; i++) {
        canvas->data[i] = color;
    }
}

void canvas_put_pixel(eCanvas* canvas, ivec2 pos, uint32_t color) 
{
#if 0
    assert(posX >= 0 && posX < canvas->width && 
           posY >= 0 && posY < canvas->height);
#endif
    if (pos.x < 0 || pos.x >= canvas->width || pos.y < 0 || pos.y >= canvas->height) {
        fprintf(stderr, "[ERAY][CANVAS OUT OF BOUNDS] pos(%d,%d) exceeds image(%d,%d)\n", 
            pos.x, pos.y, canvas->width, canvas->height);
        fflush(stdout);
        return;
    } 
    int pixel_index = (pos.y * canvas->width) + pos.x;
    canvas->data[pixel_index] = color; 
}

void canvas_destroy(eCanvas* canvas)
{
    if (canvas && canvas->data) {
        free(canvas->data);
    }
}

#if 0 
ivec2 eray_canvas_convert_2d_coordinate_system(const eCanvas* canvas, ivec2 pos)
{
    ivec2 result = ivec2_sub(canvas->origin, ((ivec2){0, 0}));
    return ivec2_add(result, pos);
}    

ivec2 eray_canvas_top_left_origin_to_center(const eCanvas* canvas, ivec2 pos)
{                            
    ivec2 result = ivec2_sub(pos, canvas->origin);
    return IVEC2(result.x, -result.y);
}
#endif


