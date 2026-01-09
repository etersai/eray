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
// “Make your code usable before you try to make it reusable”. -Casey Muratori.


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
#define EUI_HITBOX_RESIZE 16

typedef unsigned int e_ID;

#define EUI_ENABLE_BORDERS true

enum
{
    EUI_INTERACTION_NULL = 0,
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
    unsigned int size_text;
    e_UI_COLOR color_text;
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
    bool    is_left_released;
    bool    is_left_pressed;   
    int     mouse_pos_x;
    int     mouse_pos_y;
    int     mouse_delta_x;
    int     mouse_delta_y;
} e_UI_INPUT_DATA;

typedef struct {
    e_UI_FRAME   frame_buffer[EUI_FRAMES_MAX];
    size_t       frame_count;
    e_UI_THEME   theme;
    unsigned int next_id;
    e_UI_INPUT_DATA input_data;
    unsigned int    interaction_type;
    e_UI_FRAME*     hovered_over_frame;
    e_UI_FRAME*     active_use_frame;
} e_UI_CTX;

typedef struct {
    e_UI_COLOR color;
    Rect       rect; // preferably 4 ints but no time for that XD.
} eui_DrawRectInfo;

typedef struct {
    char text[1024]; // pls dont pass it more that that XD
    e_UI_COLOR color;
    int  x;
    int  y;
} eui_DrawTextInfo;

// typedef struct {
#define                          EUI_MAX_DRAW_CALLS 1024
int              draw_calls_pile[EUI_MAX_DRAW_CALLS];
eui_DrawRectInfo rect_pile[EUI_MAX_DRAW_CALLS]; 
eui_DrawTextInfo text_pile[EUI_MAX_DRAW_CALLS];
// } SynchronizedOutputData; 

// Globals.
static e_UI_THEME theme_default = {
    .size_border     = 2,
    .size_title_bar  = 20,
    .size_text       = 10,
    .color_text      = (e_UI_COLOR){ 255, 255, 255, 255},
    .color_main      = (e_UI_COLOR){ 50, 50, 50, 255},
    .color_border    = (e_UI_COLOR){ 255, 0, 0, 255},
    .color_title_bar = (e_UI_COLOR){ 20, 20, 20, 255},
};

// impl
bool eui_open_frame(e_UI_CTX* ctx, Rect rect, const char* name)
{
    perma_assert(ctx);
    perma_assert(name);

    // this is slow, later hash or smth.
    int found_idx = -1;
    for (int i = 0; i < ctx->frame_count; i++) {    
        if (strcmp(ctx->frame_buffer[i].name, name) == 0) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) { // not found. create default.
        ctx->frame_buffer[ctx->frame_count].name = name;
        ctx->frame_buffer[ctx->frame_count].opened = true;
        ctx->frame_buffer[ctx->frame_count].rect.x = rect.x;
        ctx->frame_buffer[ctx->frame_count].rect.y = rect.y;
        ctx->frame_buffer[ctx->frame_count].rect.width = rect.width;
        ctx->frame_buffer[ctx->frame_count].rect.height = rect.height;
        ctx->frame_count++;
        log_print_n_flush("[HERE!]\n");
        return true; // newly created window opened by deafult.
    }
    else
    {
#if 1
        log_print_n_flush("[Frame %s already exits [%s]]\n",
                ctx->frame_buffer[found_idx].name, stringify_bool(ctx->frame_buffer[found_idx].opened));
#endif
        return ctx->frame_buffer[found_idx].opened;
    }

}

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
        Vector2 mouse_pos           = GetMousePosition();
        input_is_left_released      = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        input_is_left_pressed       = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;

        // pass i/o input to euictx.
        ctx.input_data.mouse_pos_x   = mouse_pos.x;
        ctx.input_data.mouse_pos_y   = mouse_pos.y;
        ctx.input_data.mouse_delta_x = mouse_delta_x;
        ctx.input_data.mouse_delta_y = mouse_delta_y;
        ctx.input_data.is_left_released = input_is_left_released;
        ctx.input_data.is_left_pressed = input_is_left_pressed;

        if (ctx.input_data.is_left_released) { ctx.active_use_frame = NULL; }

        eui_open_frame(&ctx, (Rect){300, 300, 300, 300}, "window_one");
        eui_open_frame(&ctx, (Rect){400, 300, 300, 300}, "window_two");
        eui_open_frame(&ctx, (Rect){500, 300, 300, 300}, "window_three");

        // assume that frame z order is always from top to bottom (Z highest to lowest)
        for (int i = ctx.frame_count-1; i >= 0; i--) {
            if (rect_point_collision(ctx.frame_buffer[i].rect,
                                     ctx.input_data.mouse_pos_x,
                                     ctx.input_data.mouse_pos_y)) 
            {
                ctx.hovered_over_frame = &ctx.frame_buffer[i];
                break;
            }

        }

        // bug probably dereferencign null ptr. hovered_over_frame or (less likely) ctx.active_use_frame;
        if (ctx.input_data.is_left_pressed && ctx.hovered_over_frame) {

            if (rect_point_collision(ctx.hovered_over_frame->rect,
                                     ctx.input_data.mouse_pos_x,
                                     ctx.input_data.mouse_pos_y))
                {
                    ctx.active_use_frame = ctx.hovered_over_frame;
                    //ctx.interaction_type = EUI_INTERACTION_TRANSPORT;
                

                    int found_idx = -1;                                                                   
                    for (int i = 0; i < ctx.frame_count; i++) {                                        
                        if (strcmp(ctx.frame_buffer[i].name, ctx.active_use_frame->name) == 0) {
                            found_idx = i;
                            break;
                        }
                    }

                    if (found_idx == -1) { unreachable(); } // if not on list. (abort for now.)
                       
                    // cache state.
                    e_UI_FRAME temp;                               
                    temp.name = ctx.frame_buffer[found_idx].name;
                    temp.opened = ctx.frame_buffer[found_idx].opened;
                    temp.rect = ctx.frame_buffer[found_idx].rect;

                    for (int i = found_idx+1; i < ctx.frame_count; i++) {         
                        ctx.frame_buffer[i-1].rect = ctx.frame_buffer[i].rect;
                        ctx.frame_buffer[i-1].name = ctx.frame_buffer[i].name;  
                        ctx.frame_buffer[i-1].opened = ctx.frame_buffer[i].opened;  
                    }                                                                      
                    ctx.frame_buffer[ctx.frame_count-1].rect = temp.rect;                                  
                    ctx.frame_buffer[ctx.frame_count-1].opened = temp.opened;                                    
                    ctx.frame_buffer[ctx.frame_count-1].name = temp.name;                                    
                    ctx.active_use_frame = &ctx.frame_buffer[ctx.frame_count-1]; // top one becomes new active.
                }
                 
                if (ctx.active_use_frame) {

                Rect resize_rect;
                resize_rect.x = ctx.active_use_frame->rect.x + ctx.active_use_frame->rect.width - EUI_HITBOX_RESIZE;
                resize_rect.y = ctx.active_use_frame->rect.y + ctx.active_use_frame->rect.height - EUI_HITBOX_RESIZE;
                resize_rect.width  = EUI_HITBOX_RESIZE;
                resize_rect.height = EUI_HITBOX_RESIZE;

                if (rect_point_collision(resize_rect,
                                         ctx.input_data.mouse_pos_x,
                                         ctx.input_data.mouse_pos_y))
                {
                    ctx.interaction_type = EUI_INTERACTION_RESIZE;
                }
                else
                {
                    ctx.interaction_type = EUI_INTERACTION_TRANSPORT;
                }
                } // if active
        }

        if (ctx.active_use_frame) {
            if (ctx.interaction_type == EUI_INTERACTION_TRANSPORT) {
                ctx.active_use_frame->rect.x += mouse_delta_x;
                ctx.active_use_frame->rect.y += mouse_delta_y;
            }
            else if (ctx.interaction_type == EUI_INTERACTION_RESIZE) {
                ctx.active_use_frame->rect.width  += mouse_delta_x;                
                ctx.active_use_frame->rect.height += mouse_delta_y;                
                if (ctx.active_use_frame->rect.width < EUI_FRAME_MIN_WIDTH) ctx.active_use_frame->rect.width = EUI_FRAME_MIN_WIDTH;
                if (ctx.active_use_frame->rect.height < EUI_FRAME_MIN_HEIGHT) ctx.active_use_frame->rect.height = EUI_FRAME_MIN_HEIGHT;
            }
        }

        
        // prepare draw command.
        int draw_call_pile_count = 0; 
        int rect_pile_count = 0;
        int text_pile_count = 0;
        for (int i = 0; i < ctx.frame_count; i++) {

            if (ctx.frame_buffer[i].opened == true) {

                if (EUI_ENABLE_BORDERS) {
                    // register rect draw call
                    rect_pile[rect_pile_count].rect = ctx.frame_buffer[i].rect;
                    rect_pile[rect_pile_count].color = ctx.theme.color_border;

                    draw_calls_pile[draw_call_pile_count] = EUI_DRAW_COMMAND_RECT;
                    rect_pile_count++;
                    draw_call_pile_count++;
                    
                    // register rect draw call
                    rect_pile[rect_pile_count].rect.x = ctx.frame_buffer[i].rect.x + ctx.theme.size_border;
                    rect_pile[rect_pile_count].rect.y = ctx.frame_buffer[i].rect.y + ctx.theme.size_border;
                    rect_pile[rect_pile_count].rect.width = ctx.frame_buffer[i].rect.width - 2*ctx.theme.size_border;
                    rect_pile[rect_pile_count].rect.height = ctx.frame_buffer[i].rect.height - 2*ctx.theme.size_border;
                    rect_pile[rect_pile_count].color = ctx.theme.color_main;

                    draw_calls_pile[draw_call_pile_count] = EUI_DRAW_COMMAND_RECT;
                    rect_pile_count++;
                    draw_call_pile_count++;
                }
                else // NO BORDER.
                {
                    // register rect draw call
                    rect_pile[rect_pile_count].rect = ctx.frame_buffer[i].rect;
                    rect_pile[rect_pile_count].color = ctx.theme.color_main;

                    draw_calls_pile[draw_call_pile_count] = EUI_DRAW_COMMAND_RECT;
                    rect_pile_count++;
                    draw_call_pile_count++;
                }

                strncpy(text_pile[text_pile_count].text, ctx.frame_buffer[i].name, sizeof(text_pile[text_pile_count].text));
                text_pile[text_pile_count].text[sizeof(text_pile[text_pile_count].text) - 1] = '\0';

                text_pile[text_pile_count].x = ctx.frame_buffer[i].rect.x;
                text_pile[text_pile_count].y = ctx.frame_buffer[i].rect.y;
                text_pile[text_pile_count].color = ctx.theme.color_text;

                draw_calls_pile[draw_call_pile_count] = EUI_DRAW_COMMAND_TEXT;
                text_pile_count++;
                draw_call_pile_count++;
            }

        }


        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);

        BeginDrawing();
            {
                 ClearBackground(BLACK);
                 int pops_rect = 0;
                 int pops_text = 0;
                 for (int i = 0; i < draw_call_pile_count; i++) {
                    int type = draw_calls_pile[i];
                    switch (type) 
                    {
                    case EUI_DRAW_COMMAND_RECT:
                    DrawRectangle(rect_pile[pops_rect].rect.x,
                                  rect_pile[pops_rect].rect.y,
                                  rect_pile[pops_rect].rect.width,
                                  rect_pile[pops_rect].rect.height,
                                  *(Color*)&rect_pile[pops_rect].color);
                    pops_rect++;
                    break;
                    case EUI_DRAW_COMMAND_TEXT:
                    DrawText(text_pile[pops_text].text,
                             text_pile[pops_text].x,
                             text_pile[pops_text].y,
                             theme_default.size_text,
                             *(Color*)&text_pile[pops_text].color);
                    pops_text++;
                    break;
                    }    
                 } 

                 // UI
                 enum { FONT_SIZE = 20 };
                 DrawFPS(0, 0);
                 DrawText(mouse_pos_buffer, 0, 17, FONT_SIZE, LIME);
                 DrawText(mouse_delta_buffer, 0, 36, FONT_SIZE, LIME);
            }
        EndDrawing();
        // here or just before drawing??
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
