#include "../include/raylib.h"

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>

// XOR           OR       
// 0 | 0 = 0  |  0 | 0 = 0
// 1 | 0 = 1  |  1 | 0 = 1
// 0 | 1 = 1  |  0 | 1 = 1
// 1 | 1 = 0  |  1 | 1 = 1

//
// MACROS
//
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))
#define perma_assert(var) do {                                          \
    if (!(var)) {                                                       \
        fprintf(stderr, "Perma assert: %s:%d: assertion '%s' failed\n", \
        __FILE__, __LINE__, #var);                                      \
        abort();                                                        \
    }                                                                   \
} while (0)

//
// LOG 
//
#define LOG_BUFFER_BEAUTIFY_MAX 64
void log_print_beautify(const char* prefix, const char* format, ...)
{
    perma_assert(prefix);
    perma_assert(format);

    int idx = 0;
    char buffer[LOG_BUFFER_BEAUTIFY_MAX] = {0};
    while (*prefix) {
        if (idx >= LOG_BUFFER_BEAUTIFY_MAX) {
            break; 
        }
        if (*prefix >= 'a' && *prefix <= 'z') {
            buffer[idx] = *prefix^(1 << 5); // toggle case bit with xor voodoo.
        } 
        else
        {
            buffer[idx] = *prefix;
        }
        prefix++; idx++;
    } 

    FILE* out_target = stderr; 
    fprintf(out_target, "[%s]: ", buffer);
    va_list args;
    va_start(args, format);
    vfprintf(out_target, format, args);
    va_end(args);
}

void log_print_n_flush(const char* format, ...)
{
    FILE* out_target = stdout; 
    va_list args;
    va_start(args, format);
    vfprintf(out_target, format, args);
    va_end(args);
    fflush(out_target);
}

//
// HELPERS
//
const char* stringify_bool(bool var) { return var ? "TRUE" : "FALSE"; }

//
// RECT
//
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect;

Rect               rect_create(int x, int y, int width, int height);
static inline void rect_update_pos(Rect* r, int x, int y);
static inline void rect_update_pos(Rect* r, int x, int y);
static inline void rect_update_dims(Rect* r, int width, int height);
static inline void rect_update_width(Rect* r, int width);
static inline void rect_update_height(Rect* r, int height);
static inline bool rect_aabb_collision(Rect r1, Rect r2);
static inline bool rect_point_collision(Rect r, int x, int y);

Rect rect_create(int x, int y, int width, int height)
{
    assert(width > 0 && height > 0);
    Rect r = {
        .x      = x,
        .y      = y,
        .width  = width,
        .height = height,
    };
    return r;
}

static inline void rect_update_pos(Rect* r, int x, int y)
{
    r->x = x;
    r->y = y;
}                         

static inline void rect_update_dims(Rect* r, int width, int height)
{
    assert(width > 0 && height > 0);
    r->width = width;
    r->height = height;
}

static inline void rect_update_width(Rect* r, int width)
{
    rect_update_dims(r, width, r->height);
}

static inline void rect_update_height(Rect* r, int height)
{
    rect_update_dims(r, r->width, height);
}

static inline bool rect_point_collision(const Rect r, int x, int y)
{
    return rect_aabb_collision(r, (Rect){x, y, 1, 1});
}

static inline bool rect_aabb_collision(const Rect r1, const Rect r2)
{
    return (r1.x + r1.width >= r2.x && r1.x <= r2.x + r2.width        // X_AXIS
            && r1.y + r1.height >= r2.y && r1.y <= r2.y + r2.height); // Y_AXIS
}

//
// UI MEAT
//
#define EUI_FRAMES_MAX 1024
#define EUI_FRAME_MIN_WIDTH 120
#define EUI_FRAME_MIN_HEIGHT 60 
#define EUI_RESIZE_AREA_SIZE 16
enum
{
    EUI_INTERACTION_FREE = 0,
    EUI_INTERACTION_TRANSPORT,
    EUI_INTERACTION_RESIZE,
    EUI_INTERACTION_SCROLL,
};

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} e_UI_COLOR;

typedef struct {
    unsigned int size_border;
    e_UI_COLOR color_main;
    e_UI_COLOR color_border;
} e_UI_THEME;

typedef unsigned int e_ID;
typedef struct e_UI_FRAME {
    Rect         rect;
    e_ID         id;
} e_UI_FRAME; 

typedef struct {

    e_UI_FRAME   frame_buffer[EUI_FRAMES_MAX];
    e_UI_THEME   theme;
    e_UI_FRAME  *active_frame;
    unsigned int interaction_type;
    size_t       capacity;
    size_t       count;
} e_UI_CTX;

static e_UI_THEME theme_default = {
    .size_border  = 5,
    .color_main   = (e_UI_COLOR){ 100, 100, 100, 255},
    .color_border = (e_UI_COLOR){ 0, 0, 0, 255},
};

void eui_spawn_frame(e_UI_CTX* ctx, Rect rect)
{
    ctx->frame_buffer[ctx->count].rect = rect;
    ctx->frame_buffer[ctx->count].id   = ctx->count;
    ctx->count++;
}

// magnifiey
//Display *dpy = XOpenDisplay(NULL);
//Window root = DefaultRootWindow(dpy);
//XImage *img = XGetImage(dpy, root, x, y, width, height,  
//                       AllPlanes, ZPixmap);
// Scale img->data and draw to a window

// Algo in my head XD.
// if mouse button click.
// get frame that is beenng clicked on.
// determine location on that frame that is beeing clicked on.
// proceed acrodigly.

int main(void)
{
    // Platform setup
    const char* title = "Frames";
    int width = 1280;
    int height = 720;
    int target_fps = 60;
    int mouse_delta_x = 0;
    int mouse_delta_y = 0;
    InitWindow(width, height, title);
    SetTargetFPS(target_fps);
    srand(time(NULL));
      
    // UI setup
    e_UI_CTX ctx = {0};
    ctx.theme = theme_default;
    ctx.interaction_type = EUI_INTERACTION_FREE;
    ctx.capacity = EUI_FRAMES_MAX;
    
    // Spawn frames
    for (int i = 0; i < EUI_FRAMES_MAX; i++) {  
        Rect rect;
        
        rect.x = rand() % width; 
        rect.y = rand() % height;

        int w = rand() % 300;
        int h = rand() % 300;
        if (w < EUI_FRAME_MIN_WIDTH) w = EUI_FRAME_MIN_WIDTH;
        if (h < EUI_FRAME_MIN_HEIGHT) h = EUI_FRAME_MIN_HEIGHT;

        rect.width = w;
        rect.height = h;
        
        eui_spawn_frame(&ctx, rect);
        log_print_beautify("frame_count", "%d\n", ctx.count);
    }
    

    Vector2 mouse_pos_prev = {0};  
    bool input_is_left_released = false;
    bool input_is_left_pressed = false;
    while (!WindowShouldClose()) {

        Vector2 mouse_pos      = GetMousePosition();
        input_is_left_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        input_is_left_pressed  = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;

        if (ctx.active_frame != NULL && input_is_left_released) {
            ctx.interaction_type = EUI_INTERACTION_FREE;
        }

        if (ctx.interaction_type == EUI_INTERACTION_FREE) {

            ctx.active_frame = NULL;
            for (int i = ctx.count - 1; i >= 0; i--) {
                if (rect_point_collision(ctx.frame_buffer[i].rect, mouse_pos.x, mouse_pos.y)) {
                    ctx.active_frame = &ctx.frame_buffer[i];
                    break;
                }
            }

        }

        if (ctx.active_frame != NULL) {

            if (input_is_left_pressed) {

                // search for id; // IF NOT ALREADY ON TOP!.
                int found_index = -1;
                for (int i = 0; i < ctx.count; i++) {
                    if (ctx.frame_buffer[i].id == ctx.active_frame->id) {
                        found_index = i;
                        break;
                    }
                }
                if (found_index == -1) { abort(); } // if not on list.
                
                e_UI_FRAME temp;
                temp.id   = ctx.frame_buffer[found_index].id;
                temp.rect = ctx.frame_buffer[found_index].rect;

                for (int i = found_index+1; i < ctx.count; i++) {
                    ctx.frame_buffer[i-1].rect = ctx.frame_buffer[i].rect;
                    ctx.frame_buffer[i-1].id   = ctx.frame_buffer[i].id;
                }

                ctx.frame_buffer[ctx.count-1].rect = temp.rect;
                ctx.frame_buffer[ctx.count-1].id   = temp.id;
                ctx.active_frame = &ctx.frame_buffer[ctx.count-1]; // moved on top is the new active frame.

 
                Rect resize_rect;
                resize_rect.x = ctx.active_frame->rect.x + ctx.active_frame->rect.width - EUI_RESIZE_AREA_SIZE;
                resize_rect.y = ctx.active_frame->rect.y + ctx.active_frame->rect.height - EUI_RESIZE_AREA_SIZE;
                resize_rect.width  = EUI_RESIZE_AREA_SIZE;
                resize_rect.height = EUI_RESIZE_AREA_SIZE;
                    
                if (rect_point_collision(resize_rect, mouse_pos.x, mouse_pos.y)) {
                    ctx.interaction_type = EUI_INTERACTION_RESIZE;
                }
                else if (rect_point_collision(ctx.active_frame->rect, mouse_pos.x, mouse_pos.y))
                {
                    ctx.interaction_type = EUI_INTERACTION_TRANSPORT;
                }

            }

            if (ctx.interaction_type == EUI_INTERACTION_TRANSPORT) {
                ctx.active_frame->rect.x += mouse_delta_x;
                ctx.active_frame->rect.y += mouse_delta_y;
            }
            else if (ctx.interaction_type == EUI_INTERACTION_RESIZE) {
                ctx.active_frame->rect.width  += mouse_delta_x;                
                ctx.active_frame->rect.height += mouse_delta_y;                
                if (ctx.active_frame->rect.width < EUI_FRAME_MIN_WIDTH) ctx.active_frame->rect.width = EUI_FRAME_MIN_WIDTH;
                if (ctx.active_frame->rect.height < EUI_FRAME_MIN_HEIGHT) ctx.active_frame->rect.height = EUI_FRAME_MIN_HEIGHT;
            }

        }
   
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        char frame_title_buffer[BUFFER_MAX];
        char frame_rect_buffer[BUFFER_MAX];

        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
    
        if (ctx.active_frame) {
            snprintf(frame_title_buffer, BUFFER_MAX, "[FRAME ID: %d]", ctx.active_frame->id);
            snprintf(frame_rect_buffer, BUFFER_MAX, "[RECT: x:%d y:%d w:%d h:%d]", 
                    ctx.active_frame->rect.x, ctx.active_frame->rect.y, ctx.active_frame->rect.width, ctx.active_frame->rect.height);
        }

        BeginDrawing();
            {
                ClearBackground(RAYWHITE);
                
                // Draw Frames
                for (int curr_frame = 0; curr_frame < ctx.count; curr_frame++) {
                    DrawRectangle(ctx.frame_buffer[curr_frame].rect.x,
                                  ctx.frame_buffer[curr_frame].rect.y,
                                  ctx.frame_buffer[curr_frame].rect.width,
                                  ctx.frame_buffer[curr_frame].rect.height,
                                  *(Color*)&ctx.theme.color_border); // border just bigger rect underneath.

                    DrawRectangle(ctx.frame_buffer[curr_frame].rect.x + ctx.theme.size_border,
                                  ctx.frame_buffer[curr_frame].rect.y + ctx.theme.size_border,
                                  ctx.frame_buffer[curr_frame].rect.width  - 2*ctx.theme.size_border,
                                  ctx.frame_buffer[curr_frame].rect.height - 2*ctx.theme.size_border,
                                  *(Color*)&ctx.theme.color_main);
#if 0 
                    DrawRectangle(ctx.frame_buffer[curr_frame].rect.x + ctx.frame_buffer[curr_frame].rect.width - EUI_RESIZE_AREA_SIZE,
                                  ctx.frame_buffer[curr_frame].rect.y + ctx.frame_buffer[curr_frame].rect.height - EUI_RESIZE_AREA_SIZE,
                                  EUI_RESIZE_AREA_SIZE,
                                  EUI_RESIZE_AREA_SIZE,
                                  RED);
#endif
                }                 
                // UI
                enum { FONT_SIZE = 20 };
                DrawFPS(0, 0);
                DrawText(mouse_pos_buffer, 0, 17, FONT_SIZE, LIME);
                DrawText(mouse_delta_buffer, 0, 36, FONT_SIZE, LIME);
                if (ctx.active_frame) {
                DrawText(frame_title_buffer, 0, 65, FONT_SIZE, LIME); 
                DrawText(frame_rect_buffer, 0, 84, FONT_SIZE, LIME);
                }
            }
        EndDrawing();
        mouse_pos_prev = mouse_pos;
    }

    CloseWindow();
    return 0;
}

///////////////////////////
// Scratchpad and ideas. //
///////////////////////////
// int num_categories = 4;
// int category_height = ypad + 1.2 * body_font->character_height;
// float x0 = x;
// float y0 = y;
// float title_height = draw_title(x0, y0, title);
// float height = title_height + num_categories * category_height + ypad;
// my_height = height;
// y0 -= title_height;
//
// {
// y0 -= category_height;
// char *string = "Auto Snap";
// bool pressed = draw_big_text_button(x0, y0, my_width, category_height, string);
// if (pressed) do_auto_snap(this);
// }
//
// {
// y0 -= category_height;
// char *string = "Reset Orientation";
// bool pressed = draw_big_text_button(x0, y0, my_width, category_height, string);
// if (pressed) {
// // ...
// }
// }
// ...

// Cheatsheet //     
// bool IsMouseButtonPressed(int button);  // Check if a mouse button has been pressed once
// bool IsMouseButtonDown(int button);     // Check if a mouse button is being pressed
// bool IsMouseButtonReleased(int button); // Check if a mouse button has been released once
// bool IsMouseButtonUp(int button);       // Check if a mouse button is NOT being pressed
 
//// PRETTY CLEVER //
// typedef struct { int type, size; } mu_BaseCommand;
// typedef struct { mu_BaseCommand base; void *dst; } mu_JumpCommand;
// typedef struct { mu_BaseCommand base; mu_Rect rect; } mu_ClipCommand;
// typedef struct { mu_BaseCommand base; mu_Rect rect; mu_Color color; } mu_RectCommand;
// typedef struct { mu_BaseCommand base; mu_Font font; mu_Vec2 pos; mu_Color color; char str[1]; } mu_TextCommand;
// typedef struct { mu_BaseCommand base; mu_Rect rect; int id; mu_Color color; } mu_IconCommand;
//
// typedef union {
//   int type;
//   mu_BaseCommand base;
//   mu_JumpCommand jump;
//   mu_ClipCommand clip;
//   mu_RectCommand rect;
//   mu_TextCommand text;
//   mu_IconCommand icon;
// } mu_Command;
