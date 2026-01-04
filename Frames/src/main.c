#include "../include/raylib.h"

#include <stdio.h>
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

// stack
#define stack(T, n) struct { int idx; T items[n]; }
#define stack_push(stack, val) do {            \
    if ((stack).idx < arrlen((stack).items)) { \
        (stack).items[(stack).idx++] = (val);  \
    }                                          \
} while(0)

#define stack_pop(stack, val) do {          \
    if ((stack).idx > 0) {                  \
        (stack).idx--;                      \
        (val) = (stack).items[(stack).idx]; \
    }                                       \
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
// MACROS
//
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))
#define stack(T, n) struct { int idx; T items[n]; }

#define stack_push(stack, val) do {            \
    if ((stack).idx < arrlen((stack).items)) { \
        (stack).items[(stack).idx++] = (val);  \
    }                                          \
} while(0)

#define stack_pop(stack, val) do {          \
    if ((stack).idx > 0) {                  \
        (stack).idx--;                      \
        (val) = (stack).items[(stack).idx]; \
    }                                       \
} while (0)

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
#define EUI_FRAMES_MAX 64
#define RESIZE_AREA_SIZE 16
typedef struct e_UI_FRAME e_UI_FRAME;

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

struct e_UI_FRAME {
    Rect         rect;
    unsigned int id;
    bool         in_air;
    bool         in_resize;
    bool         active; // active
}; /* e_UI_FRAME */

typedef struct {
    e_UI_THEME theme;
    e_UI_FRAME frames[EUI_FRAMES_MAX]; // TODO DYNAMIC ARRAY.
    size_t     frames_count;
} e_UI_CTX;

static e_UI_THEME theme_default = {
    .size_border  = 1,
    .color_main   = (e_UI_COLOR){ 100, 100, 100, 255},
    .color_border = (e_UI_COLOR){ 0, 0, 0, 255},
};

// magnifiey
//Display *dpy = XOpenDisplay(NULL);
//Window root = DefaultRootWindow(dpy);
//XImage *img = XGetImage(dpy, root, x, y, width, height,  
//                       AllPlanes, ZPixmap);
// Scale img->data and draw to a window

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
      
    // UI setup
    e_UI_CTX ctx = {0};
    ctx.theme = theme_default;

    // Frame 1.
    ctx.frames[ctx.frames_count].rect = rect_create(600, 300, 300, 100);
    ctx.frames[ctx.frames_count].id   = ctx.frames_count;
    ctx.frames_count++;
    log_print_beautify("frame_count", "%d\n", ctx.frames_count);

    // Frame 2.
    ctx.frames[ctx.frames_count].rect = rect_create(300, 200, 300, 100);
    ctx.frames[ctx.frames_count].id   = ctx.frames_count;
    ctx.frames_count++;
    log_print_beautify("frame_count", "%d\n", ctx.frames_count);
    // frame creation WILL be immediate.
    
    Vector2 mouse_pos_prev = {0};  
    bool input_is_left_released = false;
    while (!WindowShouldClose()) {

        Vector2 mouse_pos = GetMousePosition();
        input_is_left_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;
        // if mouse button click.
        // get frame that is beenng clicked on.
        // determine location on that frame that is beeing clicked on.
        // proceed acrodigly.

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            
            for (int i = 0; i < ctx.frames_count; i++) {
                
                rect_point_collision(ctx.frames[i].rect, mouse_pos.x, mouse_pos.y) 
                

            }
        



        }


        // OLD WAY
        // Update UI.
        for (int i = 0; i < ctx.frames_count; i++) {
            
            // reset
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !input_is_left_released) {
                ctx.frames[i].active = false;
            }
            else
            {
                ctx.frames[i].in_air = false;
                ctx.frames[i].active = false;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                    Rect resize_rect;
                    resize_rect.x = ctx.frames[i].rect.x + ctx.frames[i].rect.width - RESIZE_AREA_SIZE;
                    resize_rect.y = ctx.frames[i].rect.y + ctx.frames[i].rect.height - RESIZE_AREA_SIZE;
                    resize_rect.width  = RESIZE_AREA_SIZE;
                    resize_rect.height = RESIZE_AREA_SIZE;
                    
                    if (rect_point_collision(resize_rect, mouse_pos.x, mouse_pos.y)) {
                        
                    }
                    else if (rect_point_collision(ctx.frames[i].rect, mouse_pos.x, mouse_pos.y)) {
                    
                        
                    }

            }
            
            // update
          
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ctx.frames[i].active) {

                 ctx.frames[i].in_air = true;
            }   

            if (ctx.frames[i].in_air) {
                ctx.frames[i].rect.x += mouse_delta_x;
                ctx.frames[i].rect.y += mouse_delta_y;
            }
        } 
   
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        char frame_title_buffer[BUFFER_MAX];
        char frame_rect_buffer[BUFFER_MAX];
        char frame_hovered_buffer[BUFFER_MAX];
        char frame_in_air_buffer[BUFFER_MAX];
        char frame_in_resize_buffer[BUFFER_MAX];

        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
    
        e_UI_FRAME frame = ctx.frames[0]; // hackyyy
        snprintf(frame_title_buffer, BUFFER_MAX, "[FRAME ID: %d]", frame.id);
        snprintf(frame_rect_buffer, BUFFER_MAX, "[RECT: x:%d y:%d w:%d h:%d]", 
                frame.rect.x, frame.rect.y, frame.rect.width, frame.rect.height);
        snprintf(frame_hovered_buffer, BUFFER_MAX, "[HOVERED: %s]", stringify_bool(frame.hovered));
        snprintf(frame_in_air_buffer, BUFFER_MAX, "[AIRED: %s]", stringify_bool(frame.in_air));
        snprintf(frame_in_resize_buffer, BUFFER_MAX, "[RESIZED: %s]", stringify_bool(frame.in_resize));

        BeginDrawing();
            {
                ClearBackground(RAYWHITE);
                
                // Draw Frames
                for (int curr_frame = 0; curr_frame < ctx.frames_count; curr_frame++) {
                    DrawRectangle(ctx.frames[curr_frame].rect.x,
                                  ctx.frames[curr_frame].rect.y,
                                  ctx.frames[curr_frame].rect.width,
                                  ctx.frames[curr_frame].rect.height,
                                  *(Color*)&ctx.theme.color_border); // border just bigger rect underneath.

                    DrawRectangle(ctx.frames[curr_frame].rect.x + ctx.theme.size_border,
                                  ctx.frames[curr_frame].rect.y + ctx.theme.size_border,
                                  ctx.frames[curr_frame].rect.width  - 2*ctx.theme.size_border,
                                  ctx.frames[curr_frame].rect.height - 2*ctx.theme.size_border,
                                  *(Color*)&ctx.theme.color_main);
                    
                    DrawRectangle(ctx.frames[curr_frame].rect.x + ctx.frames[curr_frame].rect.width - RESIZE_AREA_SIZE,
                                  ctx.frames[curr_frame].rect.y + ctx.frames[curr_frame].rect.height - RESIZE_AREA_SIZE,
                                  RESIZE_AREA_SIZE,
                                  RESIZE_AREA_SIZE,
                                  RED);
                    //DrawRectangle(200, 200, RESIZE_AREA_SIZE, RESIZE_AREA_SIZE, RED);
                }                 

                // UI
                enum { FONT_SIZE = 20 };
                DrawFPS(0, 0);
                DrawText(mouse_pos_buffer, 0, 17, FONT_SIZE, LIME);
                DrawText(mouse_delta_buffer, 0, 36, FONT_SIZE, LIME);
                DrawText(frame_title_buffer, 0, 55+10, FONT_SIZE, LIME);
                DrawText(frame_rect_buffer, 0, 74+10, FONT_SIZE, LIME);
                DrawText(frame_hovered_buffer, 0, 93+10, FONT_SIZE, LIME);
                DrawText(frame_in_air_buffer, 0, 112+10, FONT_SIZE, LIME);
                DrawText(frame_in_resize_buffer, 0, 131+10, FONT_SIZE, LIME);
                    
            }
        EndDrawing();
        mouse_pos_prev = mouse_pos;
    }

    CloseWindow();
    return 0;
}
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
//
//
