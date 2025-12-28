#include "../include/raylib.h"
#include "../include/microui.h"

#include "eray_canvas.h"
#include "eray_camera.h"

#include "eter_math.h"
#define ETER_DEBUG_LOG_ADD_NEW_LINE // TODO rewrite eter_debug.h
#include "eter_debug.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
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

#define unused(var) ((void) (var))

uint32_t get_random_bits_uint32_t(void)
{ // 0x00007fff masks first 15 bits;
   uint32_t r = 0;                // it looks like a slot machine :D.
    r |= ((uint32_t)(rand() & 0x00007fff) << 17);
    r |= ((uint32_t)(rand() & 0x00007fff) << 2);
    r |= ((uint32_t)(rand() & 0x00000003));
    return r;
}

typedef struct {
    fvec2 pos;
    int r;
} Sphere2D;

fvec3 cube_blank[8] = {
    {.x = 1.0f, .y = 1.0f, .z = 1.0f},
    {.x = -1.0f, .y = 1.0f, .z = 1.0f},
    {.x = -1.0f, .y = -1.0f, .z = 1.0f},
    {.x = 1.0f, .y = -1.0f, .z = 1.0f},
    {.x = 1.0f, .y = 1.0f, .z = -1.0f},
    {.x = -1.0f, .y = 1.0f, .z = -1.0f},
    {.x = -1.0f, .y = -1.0f, .z = -1.0f},
    {.x = 1.0f, .y = -1.0f, .z = -1.0f},
};

fvec3 cube[8] = {
    {.x = 1.0f, .y = 1.0f, .z = 1.0f},
    {.x = -1.0f, .y = 1.0f, .z = 1.0f},
    {.x = -1.0f, .y = -1.0f, .z = 1.0f},
    {.x = 1.0f, .y = -1.0f, .z = 1.0f},
    {.x = 1.0f, .y = 1.0f, .z = -1.0f},
    {.x = -1.0f, .y = 1.0f, .z = -1.0f},
    {.x = -1.0f, .y = -1.0f, .z = -1.0f},
    {.x = 1.0f, .y = -1.0f, .z = -1.0f},
};

Texture2D eray_create_texture_from_canvas(const eCanvas* canvas)
{
    Image img = {0};

    img.width   = canvas->width;
    img.height  = canvas->height;
    img.data    = canvas->data; 
    img.mipmaps = 1;
    img.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    return LoadTextureFromImage(img);
}


int eray_is_point_on_2d_sphere(ivec2 point, Sphere2D sphere)
{
    // (x-a)^2 + (y-b)^2 = r^2
    // inside = (dx*dx + dy*dy) <= r*r;
    int dx = point.x - (int)sphere.pos.x;
    int dy = point.y - (int)sphere.pos.y;
    return ETER_SQUARE(dx) + ETER_SQUARE(dy) <= ETER_SQUARE(sphere.r);
}

/* UI */
mu_Context ctx = {0};
#define UI_FONT_SIZE 18
int text_width(mu_Font font, const char *str, int len) { unused(font); unused(len); return MeasureText(str, UI_FONT_SIZE); }
int text_height(mu_Font font) { unused(font); return UI_FONT_SIZE; }

static const char mouse_map[3] = {
    [MOUSE_BUTTON_LEFT  ] = MU_MOUSE_LEFT,
    [MOUSE_BUTTON_RIGHT ] = MU_MOUSE_RIGHT,
    [MOUSE_BUTTON_MIDDLE] = MU_MOUSE_MIDDLE,
};

static const char key_map[16] = {
    [KEY_ENTER         & 0x0f] = MU_KEY_RETURN,
    [KEY_BACKSPACE     & 0x0f] = MU_KEY_BACKSPACE,
    [KEY_LEFT_SHIFT    & 0x0f] = MU_KEY_SHIFT,
    [KEY_LEFT_CONTROL  & 0x0f] = MU_KEY_CTRL,
    [KEY_LEFT_ALT      & 0x0f] = MU_KEY_ALT,
    [KEY_RIGHT_SHIFT   & 0x0f] = MU_KEY_SHIFT,
    [KEY_RIGHT_CONTROL & 0x0f] = MU_KEY_CTRL,
    [KEY_RIGHT_ALT     & 0x0f] = MU_KEY_ALT,
};

#define CANVAS_WIDTH  640
#define CANVAS_HEIGHT 360
#define WINDOW_WIDTH 1280 
#define WINDOW_HEIGHT 720
int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "eray");
    SetTargetFPS(60);
    srand(time(NULL)); 

    mu_init(&ctx);
    ctx.text_width = text_width;
    ctx.text_height = text_height;

    eCanvas canvas = {0};  
    canvas_initialize(&canvas, CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas.origin.x = CANVAS_WIDTH / 2;  // [320]
    canvas.origin.y = CANVAS_HEIGHT / 2; // [180] 
    canvas_fill(&canvas, 0xff000000);
         
    eCamera camera = {0}; 
    camera_set_pos(&camera, (fvec3){0.0f, 0.0f, 0.0f});   
    camera_set_focal_length(&camera, 1.0f); // 1 unit distane to image plane.

    Sphere2D spheres[8] = {0};
    float slider_value = 10.0f;

    // translate the cube. 
    for (size_t i = 0; i < ETER_ARRLEN(cube); i++) {
        cube[i].z += -3.0f;
    } 
    for (size_t i = 0; i < ETER_ARRLEN(cube); i++) {
        FVEC3_PRINT(cube[i]);
    } 

    float aspect_ratio = CANVAS_WIDTH / (float)CANVAS_HEIGHT; // 16:9
    debug_log_float(aspect_ratio); 

    for (size_t vertex = 0; vertex < ETER_ARRLEN(cube); vertex++) {
        float x_proj = (cube[vertex].x / -cube[vertex].z) / aspect_ratio;
        float y_proj = cube[vertex].y / -cube[vertex].z;
        float x_proj_remap = (x_proj + 1) / 2;
        float y_proj_remap = (y_proj + 1) / 2;
        int x_proj_pix = (int)(x_proj_remap * CANVAS_WIDTH);
        int y_proj_pix = (int)(y_proj_remap * CANVAS_HEIGHT);

        spheres[vertex].pos.x = x_proj_pix;
        spheres[vertex].pos.y = y_proj_pix;
        spheres[vertex].r = 2;

        printf("Projected vertex %zu: x:%d, y:%d\n", vertex, x_proj_pix, y_proj_pix);
    }
    
    for (int i = 0; i < ETER_ARRLEN(spheres); i++) {
        FVEC2_PRINT(spheres[i].pos);
    }
                                            
    Texture2D tex_canvas = eray_create_texture_from_canvas(&canvas);
    
    int toggleUI = 1;
    while (!WindowShouldClose())
    {
        /* INPUT */
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_wheel = GetMouseWheelMoveV();
        mu_input_mousemove(&ctx, mouse_pos.x, mouse_pos.y);
        mu_input_scroll(&ctx, 0, -mouse_wheel.y * 10);            
        for (int btn = MOUSE_BUTTON_LEFT; btn <= MOUSE_BUTTON_MIDDLE; btn++) {
            if (IsMouseButtonPressed(btn)) { mu_input_mousedown(&ctx, mouse_pos.x, mouse_pos.y, mouse_map[btn]); }
            if (IsMouseButtonReleased(btn)) { mu_input_mouseup(&ctx, mouse_pos.x, mouse_pos.y, mouse_map[btn]); }
        }
        if (IsKeyPressed(KEY_F11)) { toggleUI = !toggleUI; }
    
        /* UPDATE */
        // move cube
        for (size_t i = 0; i < ETER_ARRLEN(cube); i++) {
            cube[i].z += -0.01f;
        } 

        for (size_t vertex = 0; vertex < ETER_ARRLEN(cube); vertex++) {
            float x_proj = (cube[vertex].x / -cube[vertex].z) / aspect_ratio;
            float y_proj = cube[vertex].y / -cube[vertex].z;
            float x_proj_remap = (x_proj + 1) / 2;
            float y_proj_remap = (y_proj + 1) / 2;
            int x_proj_pix = (int)(x_proj_remap * CANVAS_WIDTH);
            int y_proj_pix = (int)(y_proj_remap * CANVAS_HEIGHT);

            spheres[vertex].pos.x = x_proj_pix;
            spheres[vertex].pos.y = y_proj_pix;
            spheres[vertex].r = (int)slider_value;
        }

        ivec2 pos = IVEC2(0, 0);
        for (pos.y=0; pos.y<canvas.height; pos.y++) {
            for (pos.x=0; pos.x<canvas.width; pos.x++) {
                for (int i = 0; i < 8; i++) {
                    int dx = pos.x - (int)spheres[i].pos.x;
                    int dy = pos.y - (int)spheres[i].pos.y;
                    int dist2 = dx*dx + dy*dy;
                    if (dist2 <= spheres[i].r*spheres[i].r) {
                        canvas_put_pixel(&canvas, pos, get_random_bits_uint32_t());
                        break;
                    }
                    else
                    {
                        canvas_put_pixel(&canvas, pos, ERAY_COLOR_BLACK);
                    }
                } 
            }
        }

        UpdateTexture(tex_canvas, canvas.data);
        
        if (toggleUI) {
            mu_begin(&ctx);
            if (mu_begin_window_ex(&ctx, "Erey options", mu_rect(10, 10, 280, 100), MU_OPT_NOCLOSE)) {
                mu_layout_row(&ctx, 2, (int[]) { 120, -1 }, 0);

                mu_label(&ctx, "Reset cube:");
                if (mu_button(&ctx, "Click!")) {
                    memcpy(cube, cube_blank, sizeof(cube));
                    // translate the cube. 
                    for (size_t i = 0; i < ETER_ARRLEN(cube); i++) {
                        cube[i].z += -3.0f;
                    } 
                }
        
                mu_label(&ctx, "Point size:");
                mu_slider(&ctx, &slider_value, 0, 25);
                mu_end_window(&ctx);
            }
            mu_end(&ctx);
        } 

        /* RENDER */
        BeginDrawing();

            ClearBackground(WHITE);
    
            // Draw canvas.
            DrawTextureEx(tex_canvas, (Vector2){0,0}, 0.0f, 2.0f, WHITE);
            
            if (toggleUI) {
                mu_Command *cmd = NULL;
                while (mu_next_command(&ctx, &cmd)) {
                    switch (cmd->type) {
                    case MU_COMMAND_TEXT: {
                        DrawText(cmd->text.str, cmd->text.pos.x, cmd->text.pos.y,
                            UI_FONT_SIZE, *(Color*)(&cmd->rect.color));
                    } break;
                    case MU_COMMAND_RECT: {
                        DrawRectangle(cmd->rect.rect.x, cmd->rect.rect.y, 
                                cmd->rect.rect.w, cmd->rect.rect.h, *(Color*)(&cmd->rect.color));                          
                    } break;
                    case MU_COMMAND_ICON:  break; // icon drawing missing!
                    case MU_COMMAND_CLIP: {
                        if (cmd->clip.rect.w == 0x1000000 || cmd->clip.rect.h == 0x1000000) {
                            EndScissorMode();
                        }
                        else
                        {
                            BeginScissorMode(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h);
                        }
                    } break;
                    }
                }
            }

            DrawFPS(0, 0); 
        EndDrawing();
    }

    UnloadTexture(tex_canvas);

    CloseWindow();
    return 0;
}
#if 0
        static int growth_rate = 1;
        sphere.r += growth_rate;
        if (sphere.r >= 100 || sphere.r <= 1) {
            growth_rate *= -1;
        }

        ivec2 pos = IVEC2(0, 0);
        for (pos.y=0; pos.y<canvas.height; pos.y++) {
            for (pos.x=0; pos.x<canvas.width; pos.x++) {
                ivec2 relative = eray_canvas_top_left_origin_to_center(&canvas, pos); 
                if (eray_is_point_on_2d_sphere(relative, sphere)) {
                    eray_put_pixel(&canvas, pos, ERAY_COLOR_BLACK);
                }
                else 
                {
                    eray_put_pixel(&canvas, pos, get_random_bits_uint32_t());
                }
            }
        }
#endif


