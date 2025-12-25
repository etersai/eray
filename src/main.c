#include "../include/raylib.h"
#include <sys/types.h>

#define ETER_VEC_STRUCT
#include "eter_math.h"
#define ETER_DEBUG_LOG_TOGGLE_NEW_LINE
#include "eter_debug.h"
#define HEHE_GIVE_BITS
#include "../hehe.h"

#include "eray_camera.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <inttypes.h>

// NOTE: Color format assumes little-endian architecture (x86, ARM) (see NOTES.txt for more info)
#define ERAY_COLOR_RED       0xff0000ff 
#define ERAY_COLOR_GREEN     0xff00ff00 
#define ERAY_COLOR_BLUE      0xffff0000 
#define ERAY_COLOR_WHITE     0xffffffff
#define ERAY_COLOR_BLACK     0xff000000
#define ERAY_COLOR_GRAY      0xff808080
#define ERAY_COLOR_DARKGRAY  0xff404040
#define ERAY_COLOR_LIGHTGRAY 0xffc0c0c0
#define ERAY_COLOR_RANDO     0xfffa6b3c

uint32_t get_random_bits_uint32_t(void)
{ // 0x00007fff masks first 15 bits;
   uint32_t r = 0;                // it looks like a slot machine :D.
    r |= ((uint32_t)(rand() & 0x00007fff) << 17);
    r |= ((uint32_t)(rand() & 0x00007fff) << 2);
    r |= ((uint32_t)(rand() & 0x00000003));
    return r;
}

#define CANVAS_WIDTH  640
#define CANVAS_HEIGHT 360

typedef struct {
    uint32_t* data; 
    ivec2 origin;    // {CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2} => [320, 180]
    int width;
    int height;
} eCanvas;

typedef struct {
    ivec2 pos;
    int r;
} Sphere2D;

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

void canvas_destroy(eCanvas* canvas)
{
    if (canvas && canvas->data) {
        free(canvas->data);
    }
}

void eray_put_pixel(eCanvas* canvas, ivec2 pos, uint32_t color) 
{
#if 0
    assert(posX >= 0 && posX < canvas->width && 
           posY >= 0 && posY < canvas->height);
#endif
    if (pos.x < 0 || pos.x >= canvas->width || pos.y < 0 || pos.y >= canvas->height) {
        fprintf(stderr, "[ERAY][OUT OF BOUNDS] pos(%d,%d) exceeds image(%d,%d)\n", 
            pos.x, pos.y, canvas->width, canvas->height);
        fflush(stdout);
        return;
    } 
    int pixel_index = (pos.y * canvas->width) + pos.x;
    canvas->data[pixel_index] = color; 
}


ivec2 eray_canvas_convert_2d_coordinate_system(const eCanvas* canvas, ivec2 pos)
{
    ivec2 result = IVEC2_SUB(canvas->origin, ((ivec2){0, 0}));
    return IVEC2_ADD(result, pos);
}    

ivec2 eray_canvas_top_left_origin_to_center(const eCanvas* canvas, ivec2 pos)
{                            
    ivec2 result = IVEC2_SUB(pos, canvas->origin); // Vec from canvas origin to whatever point at tl origin. 
    return ((ivec2){result.x, -result.y});         // Flip y to achive mathematical Y+ points up!
}

void eray_put_pixel_relative_to_origin(eCanvas* canvas, ivec2 pos, uint32_t color)
{
    //ivec2 result = IVEC2_SUB(canvas->origin, ((ivec2){0, 0}));
    //ivec2 final  = IVEC2_ADD(result, pos);
    ivec2 converted = eray_canvas_convert_2d_coordinate_system(canvas, pos);
    eray_put_pixel(canvas, converted, color); 
}

void eray_convert_canvas_to_rayimage(const eCanvas* canvas, Image* ray_image)
{
    ray_image->width   = canvas->width;
    ray_image->height  = canvas->height;
    ray_image->data    = canvas->data; 
    ray_image->mipmaps = 1;
    ray_image->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
}

Texture2D eray_canvas_texturize(const eCanvas* canvas)
{
    Image temp = {0};
    eray_convert_canvas_to_rayimage(canvas, &temp);
    return LoadTextureFromImage(temp);
}

int eray_is_point_on_2d_sphere(ivec2 point, Sphere2D sphere)
{
    // (x-a)^2 + (y-b)^2 = r^2
    // inside = (dx*dx + dy*dy) <= r*r;
    int dx = point.x - sphere.pos.x;
    int dy = point.y - sphere.pos.y;
    return ETER_SQUARE(dx) + ETER_SQUARE(dy) <= ETER_SQUARE(sphere.r);
}

#define WINDOW_WIDTH 1280 
#define WINDOW_HEIGHT 720
// CANVAS_WIDTH  640
// CANVAS_HEIGHT 360
int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "eray-trace");
    SetTargetFPS(60);
    srand(time(NULL)); 

    eCanvas canvas = {0};  
    canvas_initialize(&canvas, CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas.origin.x = CANVAS_WIDTH / 2;  // [320]
    canvas.origin.y = CANVAS_HEIGHT / 2; // [180] 
    canvas_fill(&canvas, 0xff000000);
         
    //Camera
    eCamera camera = {0}; 
    camera_set_pos(&camera, (fvec3){0.0f, 0.0f, 0.0f});   
    camera_set_aspect_ratio(&camera, (float)canvas.width/(float)canvas.height);
    
    camera_calulcate_pixel_ndc(&camera, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

    debug_log_msg_format("%f", camera.aspect_ratio);
    
    Sphere2D sphere = {((ivec2){0, 0}), 50};
           
    ivec2 pos = IVEC2(0, 0);
    for (pos.y=0; pos.y<canvas.height; pos.y++) {
        for (pos.x=0; pos.x<canvas.width; pos.x++) {
            ivec2 relative = eray_canvas_top_left_origin_to_center(&canvas, pos); 
            if (eray_is_point_on_2d_sphere(relative, sphere)) {
                eray_put_pixel(&canvas, pos, get_random_bits_uint32_t());
            }
            else 
            {
                eray_put_pixel(&canvas, pos, ERAY_COLOR_BLACK);
            }
        }
    }

    Texture2D tex_canvas = eray_canvas_texturize(&canvas);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(WHITE);
            DrawTextureEx(tex_canvas, (Vector2){0,0}, 0.0f, 1.0f, WHITE);
            DrawFPS(0, 0); 
        EndDrawing();
    }


    UnloadTexture(tex_canvas);

    CloseWindow();
    return 0;
}
