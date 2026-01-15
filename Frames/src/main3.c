#include "../include/raylib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

//
// MACROS
//
#define eray_norm(value, min, max) (((value) - (min)) / ((max) - (min)))
#define eray_lerp(norm, min, max) (((max) - (min)) * (norm) + (min))
// MAP is the combianation of norm and lerp. (MAP ONE RANGE TO ANOTHER ONE)
#define eray_min(a, b) ((a) < (b) ? (a) : (b))
#define eray_max(a, b) ((a) > (b) ? (a) : (b))
#define eray_clamp(val, lo, hi) (eray_min(eray_max((val), (lo)), (hi)))
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct { int x; int y; } vec2i; 
typedef struct { int x; int y; } vec2f; 

#define expect(var) do {                                          \
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
    expect(prefix);
    expect(format);

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
static inline float clamp_me_float(float val, float min, float max) {
    if      (val > max) {return max;}
    else if (val < min) {return min;}
    else                {return val;}
}

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
    assert(width >= 0 && height >= 0);
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

// DEBUG.
void log_rect_print_n_flush(const Rect r)
{
    fprintf(stdout, "[RECT X:%d, Y:%d, WIDTH:%d, HEIGHT:%d]\n", r.x, r.y, r.width, r.height);
    fflush(stdout);
}

void draw_rect_outline(Rect r, Color color)
{
    DrawLine(r.x, r.y, r.x+r.width, r.y, color);
    DrawLine(r.x+r.width, r.y, r.x+r.width, r.y+r.height, color);
    DrawLine(r.x+r.width, r.y+r.height, r.x, r.y + r.height, color); 
    DrawLine(r.x, r.y + r.height, r.x, r.y, color);
}

//
// CONSTANTS
//
#define EUI_MAX_FRAMES 1024
#define EUI_MAX_DRAWCALLS 2048
#define EUI_MAX_TEXT_LEN 1024
#define EUI_INVERSE_SCROLL true

#define UI_BASE_W 1280
#define UI_BASE_H 720

//
// THEME
//
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} e_UI_COLOR;

typedef struct {
    e_UI_COLOR color_text;
    e_UI_COLOR color_main;
    e_UI_COLOR color_bar;
    e_UI_COLOR color_border;
    e_UI_COLOR color_button_close;
    e_UI_COLOR color_button_zip;
} e_UI_THEME;

static const e_UI_THEME theme_default = {
    .color_text      = (e_UI_COLOR){ 255, 165, 0, 255},
    .color_main      = (e_UI_COLOR){ 50, 50, 50, 255},
    .color_border    = (e_UI_COLOR){ 255, 165, 0, 255},
    .color_bar = (e_UI_COLOR){ 20, 20, 20, 255},
    .color_button_close = (e_UI_COLOR){ 255, 0, 0, 255},
    .color_button_zip = (e_UI_COLOR){ 255, 255, 0, 255},
};

enum // INTERACTIONS
{
    EUI_INTERACTION_NONE = 0,
    EUI_INTERACTION_DRAG,
    EUI_INTERACTION_RESIZE,
};

enum // DRAWCALL TYPES
{
    EUI_DRAWCALL_RECT = 0,
    EUI_DRAWCALL_TEXT,
};

typedef struct {
    e_UI_COLOR color;
    Rect       rect; // preferably 4 ints but no time for that XD. (NOTE: will regret it XD)
} eui_DrawRectInfo;

typedef struct {
    char text[EUI_MAX_TEXT_LEN]; // pls dont pass it more that that XD
    e_UI_COLOR color;
    int  x; // Assumed first letter pos at top-left corner.
    int  y;
    float font_size; // already scaled. (don't know if it's the right choice but will see)
}eui_DrawTextInfo;

typedef struct {
    // counts should get wiped each 'tick' of my ui 
    int count;
    int count_rect_pile;   
    int count_text_pile;
    int              draw_calls_pile[EUI_MAX_DRAWCALLS];
    eui_DrawRectInfo       rect_pile[EUI_MAX_DRAWCALLS]; 
    eui_DrawTextInfo       text_pile[EUI_MAX_DRAWCALLS];
} eui_SynchronizedOutputData; 

static const int DEFAULT_BORDER_OFFSET = 2;
static const int DEFAULT_BAR_HEIGHT = 32;
static const int DEFAULT_FRAME_WIDTH = 420;
static const int DEFAULT_FRAME_HEIGHT = 300;
static const int DEFAULT_BUTTON_SIZE = 16; 
// should probably divide all the defaults by global scale.

typedef struct {
    int bar_height;
    int border_offset;
    int button_size;
} eui_ScaledDefaults;

typedef struct { 
    Rect master;
    Rect bar; // |-| .,, |& . In memory of our pullup BAR.
    Rect main; 
    Rect button_close;
    Rect button_zip;
    Rect button_resize;
    vec2i text_pos; 
    float text_size;
} eui_Hitbox; 

typedef struct {
    int width;
    int height;
} e_UI_TEXT_DIMS;

typedef struct {
    bool    is_left_released;
    bool    is_left_pressed;   
    int     mouse_pos_x;
    int     mouse_pos_y;
    float   mouse_scroll_x;
    float   mouse_scroll_y;
    int     mouse_delta_x;
    int     mouse_delta_y;
} e_UI_INPUT_DATA;

typedef struct {
    vec2f canvas_size;
    vec2f canvas_delta;
    Rect rect;
    Rect rect_previous;
    bool opened;
    bool zipped;
    const char* name; // it's id for now.
    float scale; // 1.0f default.
} e_UI_FRAME;


typedef struct {
    eui_SynchronizedOutputData draw_calls;
    e_UI_FRAME   frame_buffer[EUI_MAX_FRAMES];
    size_t       frame_count;
    e_UI_THEME   theme;
    float        global_scale;
    int          user_font_size;
    eui_ScaledDefaults default_sizes;
    e_UI_INPUT_DATA input_data;
    unsigned int interaction_type;
    e_UI_FRAME*  active_frame;
    e_UI_FRAME*  hovered_frame;
    e_UI_TEXT_DIMS (*eui_get_text_metrics)(const char* text, int font_size);
} e_UI_CTX;

int eui_init(e_UI_CTX* ctx)
{
    expect(ctx);
    expect(ctx->eui_get_text_metrics);
    expect(ctx->user_font_size > 0);
    ctx->theme = theme_default;
    ctx->global_scale = 1.0f;
    return 0;
}

void eui_update_ui_scale(e_UI_CTX* ctx, int window_width, int window_height)
{
    float ui_scale = fminf((float)window_width / UI_BASE_W, (float)window_height / UI_BASE_H);
    ctx->global_scale = ui_scale;
}

void eui_start_frame(e_UI_CTX* ctx)
{
    if (ctx->input_data.is_left_released) {
        ctx->interaction_type = EUI_INTERACTION_NONE;
        ctx->active_frame = NULL;
    }
    ctx->hovered_frame = NULL;

    // reset drawcalls piles.
    ctx->draw_calls.count = 0;
    ctx->draw_calls.count_rect_pile = 0;
    ctx->draw_calls.count_text_pile = 0;
}

bool eui_open_frame(e_UI_CTX* ctx, Rect rect, const char* name)
{
    expect(ctx);
    expect(name);

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
        ctx->frame_buffer[ctx->frame_count].rect = rect;
        ctx->frame_buffer[ctx->frame_count].scale = 1.0f;
        ctx->frame_buffer[ctx->frame_count].opened = true;
        ctx->frame_buffer[ctx->frame_count].zipped = false;
        ctx->frame_count++;
        return true; // newly created window opened by deafult.
    }
    else
    {
#if 0
        log_print_n_flush("[Frame %s already exits [%s]]\n",
                ctx->frame_buffer[found_idx].name, stringify_bool(ctx->frame_buffer[found_idx].opened));
#endif
        return ctx->frame_buffer[found_idx].opened;
    }

}

void eui_calculate_hitboxes(e_UI_CTX* ctx, e_UI_FRAME* frame, eui_Hitbox* hitbox)
{
    // Master hibox derved from frame logical size rect.
    hitbox->master.x = frame->rect.x;
    hitbox->master.y = frame->rect.y;
    hitbox->master.width = frame->rect.width * ctx->global_scale;
    hitbox->master.height = frame->rect.height * ctx->global_scale;
    
    // DragBar
    hitbox->bar.x = hitbox->master.x + DEFAULT_BORDER_OFFSET; 
    hitbox->bar.y = hitbox->master.y + DEFAULT_BORDER_OFFSET;
    hitbox->bar.width = hitbox->master.width - DEFAULT_BORDER_OFFSET*2;
    hitbox->bar.height = DEFAULT_BAR_HEIGHT * frame->scale;
     
    // BUTTONS 
    int button_size = DEFAULT_BUTTON_SIZE * frame->scale;
    int button_slot_y_axis = hitbox->master.y + hitbox->bar.height/2 + DEFAULT_BORDER_OFFSET;
    int button_slot_0_x = hitbox->master.x + hitbox->master.width + DEFAULT_BORDER_OFFSET - button_size;
    int button_slot_1_x = hitbox->master.x + hitbox->master.width +DEFAULT_BORDER_OFFSET - 2*button_size;

    // close
    hitbox->button_close.x = button_slot_0_x - button_size/2;            
    hitbox->button_close.y = button_slot_y_axis - button_size/2;             
    hitbox->button_close.width = button_size;            
    hitbox->button_close.height = button_size;            
    
    // zip
    hitbox->button_zip.x = button_slot_1_x - button_size;            
    hitbox->button_zip.y = button_slot_y_axis - button_size/2;             
    hitbox->button_zip.width = button_size;            
    hitbox->button_zip.height = button_size;            
    
    Rect HITBOX_NULL = {0};

    if (frame->zipped) {  
        hitbox->main          = HITBOX_NULL;
        hitbox->button_resize = HITBOX_NULL;
    }
    else
    {
        hitbox->main.x = hitbox->master.x + DEFAULT_BORDER_OFFSET; 
        hitbox->main.y = hitbox->master.y + hitbox->bar.height + 2*DEFAULT_BORDER_OFFSET;
        hitbox->main.width = hitbox->master.width - 2*DEFAULT_BORDER_OFFSET;
        hitbox->main.height = hitbox->master.height - hitbox->bar.height - 3*DEFAULT_BORDER_OFFSET;

        hitbox->button_resize.x = hitbox->master.x + hitbox->master.width - button_size; 
        hitbox->button_resize.y = hitbox->master.y + hitbox->master.height - button_size;
        hitbox->button_resize.width = button_size;
        hitbox->button_resize.height = button_size;
    }

    e_UI_TEXT_DIMS text_dimensions = ctx->eui_get_text_metrics(frame->name, ctx->user_font_size*frame->scale);
    vec2i text_pos;
    text_pos.x = hitbox->bar.x + 5;
    text_pos.y = hitbox->bar.y + (hitbox->bar.height - text_dimensions.height)/2;
    hitbox->text_pos = text_pos;
    hitbox->text_size = ctx->user_font_size*frame->scale;

}

void eui_end_frame(e_UI_CTX* ctx)
{
    // Scan for hovered over frames.
    for (int i = ctx->frame_count-1; i >= 0; i--) {

        if (rect_point_collision((Rect){ctx->frame_buffer[i].rect.x,
                                        ctx->frame_buffer[i].rect.y,
                                        ctx->frame_buffer[i].rect.width * ctx->global_scale,
                                        ctx->frame_buffer[i].rect.height * ctx->global_scale},
                             ctx->input_data.mouse_pos_x,
                             ctx->input_data.mouse_pos_y)) {
            ctx->hovered_frame = &ctx->frame_buffer[i];
            break;
        }
    }
    
    if (ctx->hovered_frame) {

        eui_Hitbox hitbox = {0};
        eui_calculate_hitboxes(ctx, ctx->hovered_frame, &hitbox);

        bool over_main = rect_point_collision(hitbox.main, ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y);
        bool over_bar = rect_point_collision(hitbox.bar, ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y);

        
        if (over_main) {
            ctx->hovered_frame->canvas_delta.x += ctx->input_data.mouse_scroll_x * 2.0f;
            ctx->hovered_frame->canvas_delta.y += ctx->input_data.mouse_scroll_y * 2.0f;
        }

            

        if (over_bar) {
            log_print_n_flush("OVER BARY\n");
        }


                // const float SCALE_SENSITIVITY = 0.1f;
                // const float SCALE_MIN = 1.0f;
                // const float SCALE_MAX = 2.0f;
                // float scroll = ctx.input_data.mouse_scroll_y;
                // if (fabsf(scroll) > 0.1f) {
                //     if (EUI_INVERSE_SCROLL) {
                //         ctx.hovered_frame->scale += -scroll * SCALE_SENSITIVITY;
                //     }
                //     else 
                //     {
                //         ctx.hovered_frame->scale += scroll * SCALE_SENSITIVITY;
                //     }
                //     ctx.hovered_frame->scale = clamp_me_float(ctx.hovered_frame->scale, SCALE_MIN, SCALE_MAX);
                // }
                //
        


        if (ctx->input_data.is_left_pressed) {

            ctx->active_frame = ctx->hovered_frame;

            if (rect_point_collision(hitbox.button_close, ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                log_print_n_flush("CLOSE!\n");
            }
            else if (rect_point_collision(hitbox.button_zip, ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                    ctx->active_frame->zipped = !ctx->active_frame->zipped;
                    if (ctx->active_frame->zipped) {
                        ctx->active_frame->rect_previous = ctx->active_frame->rect;
                        ctx->active_frame->rect.height = (DEFAULT_BAR_HEIGHT/ctx->global_scale) * ctx->active_frame->scale + (2*(DEFAULT_BORDER_OFFSET/ctx->global_scale));
                    }
                    else
                    {
                        ctx->active_frame->rect.height = ctx->active_frame->rect_previous.height;
                    }
                    log_print_n_flush("ZIP!\n");
            }
            else if (rect_point_collision(hitbox.button_resize,ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                ctx->interaction_type = EUI_INTERACTION_RESIZE;
                log_print_n_flush("RESIZE!\n");
            }
            else if (rect_point_collision(hitbox.main,ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                log_print_n_flush("MAIN!\n");
            }
            else if (rect_point_collision(hitbox.bar,ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                ctx->interaction_type = EUI_INTERACTION_DRAG;
                log_print_n_flush("BAR!\n");
            }
            else if (rect_point_collision(hitbox.master,ctx->input_data.mouse_pos_x, ctx->input_data.mouse_pos_y)) {
                log_print_n_flush("MASTER!\n");
            }

        }


    }

    if (ctx->active_frame) {

        if (ctx->interaction_type == EUI_INTERACTION_DRAG) {
            ctx->active_frame->rect.x += ctx->input_data.mouse_delta_x;
            ctx->active_frame->rect.y += ctx->input_data.mouse_delta_y;
        }
        else if (ctx->interaction_type == EUI_INTERACTION_RESIZE) {
            ctx->active_frame->canvas_size.x+= 1;
                ctx->active_frame->rect.width  += ctx->input_data.mouse_delta_x/ctx->global_scale;                
                ctx->active_frame->rect.height += ctx->input_data.mouse_delta_y/ctx->global_scale;                
                if (ctx->active_frame->rect.width < 100) 
                    ctx->active_frame->rect.width = 100;
                if (ctx->active_frame->rect.height < 155) 
                    ctx->active_frame->rect.height = 155;
            }

    }



}


void eui_update_scale_defaults(e_UI_CTX* ctx)
{

}


// renderer specific thigies.
static e_UI_TEXT_DIMS ray_get_text_metrics(const char* text, int font_size)
{
    Font font = GetFontDefault();
    Vector2 metrics = MeasureTextEx(font, text, font_size, 0.0f);    
    return (e_UI_TEXT_DIMS){ .width = metrics.x, .height = metrics.y };
}

#define WORK
int main(void)
{
#if defined(WORK) 
    const int screenWidth = 1280;
    const int screenHeight = 720;
#else
    const int screenWidth = 1920;
    const int screenHeight = 1080;
#endif
    InitWindow(screenWidth, screenHeight, "programmingEASY");
    SetTargetFPS(120);

    // UI INIT STEPS
    e_UI_CTX ctx = {0};
    ctx.eui_get_text_metrics = ray_get_text_metrics;
    ctx.user_font_size = 16;
    eui_init(&ctx);
    eui_update_ui_scale(&ctx, screenWidth, screenHeight);

    ctx.frame_buffer[0].canvas_size.x = 500;
    ctx.frame_buffer[0].canvas_size.y = 500;

    // Platform input init
    Vector2 mouse_pos_prev = {0};  
    int     mouse_delta_x = 0;
    int     mouse_delta_y = 0;
    bool    input_is_left_released = false;
    bool    input_is_left_pressed = false;
    while (!WindowShouldClose())
    {
        Vector2 mouse_pos           = GetMousePosition();
        Vector2 mouse_scroll        = GetMouseWheelMoveV();  
        input_is_left_released      = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        input_is_left_pressed       = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;
        
        // UI REGISTER INPUT DATA. assert(capture at the beginning)
        ctx.input_data.mouse_pos_x = mouse_pos.x;
        ctx.input_data.mouse_pos_y = mouse_pos.y;
        ctx.input_data.mouse_delta_x = mouse_delta_x;
        ctx.input_data.mouse_delta_y = mouse_delta_y;
        ctx.input_data.mouse_scroll_x = mouse_scroll.x;
        ctx.input_data.mouse_scroll_y = mouse_scroll.y;
        ctx.input_data.is_left_pressed = input_is_left_pressed;
        ctx.input_data.is_left_released = input_is_left_released;

        eui_start_frame(&ctx);
        if (eui_open_frame(&ctx, (Rect){100, 100, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT}, "My Frame")) {
            // Do stuff here etc.
        }
        eui_end_frame(&ctx);

        // DEBUG_UI_BUFFER_UPDATE //
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        char mouse_scroll_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
        snprintf(mouse_scroll_buffer, BUFFER_MAX, "[MOUSE SCROLL: %.2f, %.2f]", mouse_scroll.x, mouse_scroll.y);

        eui_Hitbox hitbox = {0};
        eui_calculate_hitboxes(&ctx, &ctx.frame_buffer[0], &hitbox);

        log_print_n_flush("Acumulated delta: %d, %d\n", ctx.frame_buffer[0].canvas_delta.x, ctx.frame_buffer[0].canvas_delta.y);

        Rect frame_canvas = (Rect){0, 0, ctx.frame_buffer[0].canvas_size.x, ctx.frame_buffer[0].canvas_size.y};
        frame_canvas.x = hitbox.main.x + ctx.frame_buffer[0].canvas_delta.x;
        frame_canvas.y = hitbox.main.y + ctx.frame_buffer[0].canvas_delta.y;
    
        log_print_n_flush("Acumulated delta: %d, %d\n", frame_canvas.x, frame_canvas.y);

        BeginDrawing();
        {
            ClearBackground(BLACK);

#if 1       // Debug hitboxes.
            Color DEBUG_COLOR = RED;
            draw_rect_outline(hitbox.master, DEBUG_COLOR);
            draw_rect_outline(hitbox.bar, DEBUG_COLOR);
            draw_rect_outline(hitbox.main, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_close, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_zip, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_resize, DEBUG_COLOR);
            
            draw_rect_outline(frame_canvas, MAGENTA);

            DrawText(ctx.frame_buffer[0].name, hitbox.text_pos.x, hitbox.text_pos.y, hitbox.text_size, RED);
#endif
            
            enum { FONT_SIZE = 20 };
            DrawFPS(0, 0);
            DrawText(mouse_pos_buffer, 0, 17, FONT_SIZE, LIME);
            DrawText(mouse_delta_buffer, 0, 36, FONT_SIZE, LIME);
            DrawText(mouse_scroll_buffer, 0, 55, FONT_SIZE, LIME);
        }
        EndDrawing();
        mouse_pos_prev = mouse_pos;
    }
    
    CloseWindow();
    
    return 0;
}
