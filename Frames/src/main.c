#include "../include/raylib.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

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
#define unreachable() do {                                                  \
    fprintf(stderr, "Unreachable code hit at %s:%d\n", __FILE__, __LINE__); \
    abort();                                                                \
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
#define EUI_RESIZE_AREA_SIZE 16 // hitbox.

#define EUI_ENABLE_BORDERS true

enum
{
    EUI_INTERACTION_FREE = 0,
    EUI_INTERACTION_TRANSPORT,
    EUI_INTERACTION_RESIZE,
    EUI_INTERACTION_SCROLL,
};

enum
{
    EUI_DRAW_COMMAND_RECT = 0,
    EUI_DRAW_COMMAND_TEXT,
};

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} e_UI_COLOR;

typedef struct {
    unsigned int size_border;
    unsigned int size_title_bar;
    e_UI_COLOR color_main;
    e_UI_COLOR color_border;
    e_UI_COLOR color_title_bar;
} e_UI_THEME;

typedef struct {
    Rect rect;
    bool opened;
    const char* name; // it's id for now.
} e_UI_FRAME; 

typedef struct {
    e_UI_FRAME   frame_buffer[EUI_FRAMES_MAX];
    size_t       frame_count;
    e_UI_THEME   theme;
} e_UI_CTX;

typedef struct {
    e_UI_COLOR color;
    Rect       rect; // preferably 4 ints but no time for that XD.
} eui_DrawRectInfo;

typedef struct {
    e_UI_COLOR color;
    int x;
    int y;
} eui_DrawTextInfo;

#define                     EUI_MAX_DRAW_CALLS 1024
int              draw_calls[EUI_MAX_DRAW_CALLS];
eui_DrawRectInfo draw_info_rect[EUI_MAX_DRAW_CALLS]; 
eui_DrawTextInfo draw_info_text[EUI_MAX_DRAW_CALLS];

// Globals.
static e_UI_THEME theme_default = {
    .size_border     = 2,
    .size_title_bar  = 20,
    .color_main      = (e_UI_COLOR){ 50, 50, 50, 255},
    .color_border    = (e_UI_COLOR){ 255, 0, 0, 255},
    .color_title_bar = (e_UI_COLOR){ 20, 20, 20, 255},
};

int main(void)
{
    // Platform setup
    const char* title = "Frames";
    int window_width = 1280;
    int window_height = 720;
    int target_fps = 60;
    int mouse_delta_x = 0;
    int mouse_delta_y = 0;
    InitWindow(window_width, window_height, title);
    SetTargetFPS(target_fps);
    srand(time(NULL));
      
    // UI setup
    e_UI_CTX ctx = {0};
    ctx.theme    = theme_default;
    
    Vector2 mouse_pos_prev = {0};  
    bool    input_is_left_released = false;
    bool    input_is_left_pressed = false;
    while (!WindowShouldClose()) {

// The algorithm is conceptually:
// Compute ID
// Lookup ID in state storage
// If found → return state
// If not → allocate default state and return it

        Vector2 mouse_pos           = GetMousePosition();
        input_is_left_released      = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        input_is_left_pressed       = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;


        int dcs_count = 0; 
        int dsc_count_rect = 0;
        int dsc_count_text = 0;

        // put_window_up
        int w = 300;
        int h = 300;
        int x = 300;
        int y = 300;
        const char* name = "window";

        bool found = false;
        int found_idx = -1;
        for (int i = 0; i < arrlen(ctx.frame_buffer); i++) {    
            if (ctx.frame_buffer[i].name != NULL && 
                strcmp(ctx.frame_buffer[i].name, name) == 0) {
                found = true;
                found_idx = i;
                break;
            }
        }
        if (!found) {
            ctx.frame_buffer[ctx.frame_count].name = name;
            ctx.frame_buffer[ctx.frame_count].opened = true;
            ctx.frame_buffer[ctx.frame_count].rect = rect_create(x, y, w, h); 
            ctx.frame_count++;
        }
        else
        {
            printf("[%s, %s]\n", ctx.frame_buffer[found_idx].name, stringify_bool(ctx.frame_buffer[found_idx].opened));
        }

        // prepare draw command.
        if (ctx.frame_buffer[found_idx].opened == true) {

            eui_DrawRectInfo rect_info;
            eui_DrawTextInfo text_info;

            if (EUI_ENABLE_BORDERS) {
                draw_calls[dcs_count] = EUI_DRAW_COMMAND_RECT;
                draw_info_rect[dsc_count_rect].rect = ctx.frame_buffer[found_idx].rect;
                draw_info_rect[dsc_count_rect].color = ctx.theme.color_border;
            }

            
            draw_calls[dcs_count++] = EUI_DRAW_COMMAND_RECT;
            draw_info_rect[dsc_count_rect].rect = rect_info.rect;
            draw_info_rect[dsc_count_rect].color = rect_info.color;


        }
        

        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);

        BeginDrawing();
            {
                 ClearBackground(RAYWHITE);
                 // UI
                 enum { FONT_SIZE = 20 };
                 DrawFPS(0, 0);
                 DrawText(mouse_pos_buffer, 0, 17, FONT_SIZE, LIME);
                 DrawText(mouse_delta_buffer, 0, 36, FONT_SIZE, LIME);
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
