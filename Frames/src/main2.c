#include "../include/raylib.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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

static const int DEFAULT_BORDER_OFFSET = 2;
static const int DEFAULT_BAR_HEIGHT = 32;
static const int DEFAULT_FRAME_WIDTH = 420;
static const int DEFAULT_FRAME_HEIGHT = 300;
static const int DEFAULT_BUTTON_SIZE = 16; 
static const int DEFAULT_MIN_WIDTH = 256;
static const int DEFAULT_MIN_HEIGHT = 128;
static const float DEAFULT_UI_SCALE = 1.0f;

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
    Rect rect;
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
    int          user_font_size;
    e_UI_INPUT_DATA input_data;
    e_UI_FRAME*  held_frame;
    e_UI_FRAME*  active_frame;
    e_UI_FRAME*  hovered_frame;
    e_UI_TEXT_DIMS (*eui_get_text_metrics)(const char* text, int font_size);
} e_UI_CTX;


void eui_calculate_hitboxes(e_UI_CTX* ctx, e_UI_FRAME* frame, eui_Hitbox* hitbox)
{
    expect(frame);
    expect(hitbox);

    Rect hitbox_null = {0};

    // HITBOX EVERYTHING.
    hitbox->master.x = frame->rect.x;
    hitbox->master.y =  frame->rect.y;
    hitbox->master.width = DEFAULT_FRAME_WIDTH * frame->scale;
    if (frame->zipped) { hitbox->master.height = DEFAULT_BAR_HEIGHT*frame->scale + 2*DEFAULT_BORDER_OFFSET; }
    else { hitbox->master.height = DEFAULT_FRAME_HEIGHT*frame->scale; }
    
    // HITBOX DRAGBAR
    hitbox->bar.x = hitbox->master.x + DEFAULT_BORDER_OFFSET; 
    hitbox->bar.y = hitbox->master.y + DEFAULT_BORDER_OFFSET;
    hitbox->bar.width = hitbox->master.width - DEFAULT_BORDER_OFFSET*2;
    hitbox->bar.height = DEFAULT_BAR_HEIGHT * frame->scale;
   
    // HITBOX MAIN
    if (frame->zipped) {
        hitbox->main = hitbox_null;
    }
    else
    {
        hitbox->main.x = hitbox->master.x + DEFAULT_BORDER_OFFSET; 
        hitbox->main.y = hitbox->master.y + hitbox->bar.height + 2*DEFAULT_BORDER_OFFSET;
        hitbox->main.width = hitbox->master.width - 2*DEFAULT_BORDER_OFFSET;
        hitbox->main.height = hitbox->master.height - hitbox->bar.height - 3*DEFAULT_BORDER_OFFSET;
    }

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

    // resize (bottom right location).
    if (frame->zipped) {  
        hitbox->button_resize = hitbox_null;
    }
    else
    {
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

    // IMPORTANT !!!
    frame->rect = hitbox->master; // assign recalculated hitbox as the main.
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
        ctx->frame_buffer[ctx->frame_count].rect = rect;
        ctx->frame_buffer[ctx->frame_count].name = name;
        ctx->frame_buffer[ctx->frame_count].scale = DEAFULT_UI_SCALE;
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

void eui_hitbox_data_transform_to_draw_calls(e_UI_CTX* ctx, eui_Hitbox* hitbox)
{
    // TOFUCKINGDO!
}

int eui_init(e_UI_CTX* ctx)
{
    expect(ctx);
    expect(ctx->eui_get_text_metrics);
    expect(ctx->user_font_size > 0);
    ctx->theme = theme_default;
    return 0;
}

// renderer specific thigies.
static e_UI_TEXT_DIMS ray_get_text_metrics(const char* text, int font_size)
{
    Font font = GetFontDefault();
    Vector2 metrics = MeasureTextEx(font, text, font_size, 0.0f);    
    return (e_UI_TEXT_DIMS){ .width = metrics.x, .height = metrics.y };
}

// nifty trick for the record, maybe i will find an usecase for it :D.
//Rect* as_array = (Rect*)&hitbox;
//size_t num_hitbox = sizeof(eui_Hitbox)/sizeof(Rect);
//
//

int main(void)
{
    const int screenWidth = 1920/2;
    const int screenHeight = 1080/2;
    InitWindow(screenWidth, screenHeight, "RewriteKinda");
    //SetTargetFPS(120);

    // UI INIT STEPS
    e_UI_CTX ctx = {0};
    ctx.active_frame = &ctx.frame_buffer[0]; // HACKY
    ctx.eui_get_text_metrics = ray_get_text_metrics;
    ctx.user_font_size = 16;
    eui_init(&ctx);

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
        
        // USER REGISTERED DATA STRUCT.
        // UI REGISTER INPUT DATA. assert(capture before first frame spawn)
        ctx.input_data.mouse_pos_x = mouse_pos.x;
        ctx.input_data.mouse_pos_y = mouse_pos.y;
        ctx.input_data.mouse_delta_x = mouse_delta_x;
        ctx.input_data.mouse_delta_y = mouse_delta_y;
        ctx.input_data.mouse_scroll_x = mouse_scroll.x;
        ctx.input_data.mouse_scroll_y = mouse_scroll.y;
        ctx.input_data.is_left_pressed = input_is_left_pressed;
        ctx.input_data.is_left_released = input_is_left_released;

        // drop frame if releaased and holding something 
        if (input_is_left_released && ctx.held_frame != NULL) { ctx.held_frame = NULL; }
        if (input_is_left_released && ctx.active_frame != NULL) { ctx.active_frame = NULL; }
        ctx.hovered_frame = NULL;

        ctx.draw_calls.count = 0;
        ctx.draw_calls.count_rect_pile = 0;
        ctx.draw_calls.count_text_pile = 0;

        Rect DEFAULT_RECT = (Rect){300, 300, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT}; 
        if (eui_open_frame(&ctx, DEFAULT_RECT, "eray")) {
            // do your stuff here.
        }

#if 1
        static eui_Hitbox hitbox = {0}; // XD
        for (int i = ctx.frame_count-1; i >= 0; i--) {
            
            if (rect_point_collision(ctx.frame_buffer[i].rect,
                                 ctx.input_data.mouse_pos_x,
                                 ctx.input_data.mouse_pos_y)) {
                ctx.hovered_frame = &ctx.frame_buffer[i];
            }
            
            eui_calculate_hitboxes(&ctx, &ctx.frame_buffer[i], &hitbox);

            if (rect_point_collision(hitbox.bar, mouse_pos.x, mouse_pos.y) && ctx.hovered_frame) {
                const float SCALE_SENSITIVITY = 0.1f;
                const float SCALE_MIN = 1.0f;
                const float SCALE_MAX = 2.0f;
                float scroll = ctx.input_data.mouse_scroll_y;
                if (scroll != 0.0f) {
                    if (EUI_INVERSE_SCROLL) {
                        ctx.hovered_frame->scale += -scroll * SCALE_SENSITIVITY;
                    }
                    else 
                    {
                        ctx.hovered_frame->scale += scroll * SCALE_SENSITIVITY;
                    }
                    ctx.hovered_frame->scale = clamp_me_float(ctx.hovered_frame->scale, SCALE_MIN, SCALE_MAX);
                }
            }

            if (input_is_left_pressed && ctx.hovered_frame) {
               
                log_print_n_flush("[HEH]\n");
                ctx.active_frame = ctx.hovered_frame;

                if (rect_point_collision(hitbox.button_close, mouse_pos.x, mouse_pos.y)) {
                log_print_n_flush("CLOSE!\n");
                }
                else if (rect_point_collision(hitbox.button_zip, mouse_pos.x, mouse_pos.y)) {
                ctx.active_frame->zipped = !ctx.active_frame->zipped;
                log_print_n_flush("ZIP!\n");
                }
                else if (rect_point_collision(hitbox.button_resize, mouse_pos.x, mouse_pos.y)) {
                log_print_n_flush("RESIZE!\n");
                }
                else if (rect_point_collision(hitbox.main, mouse_pos.x, mouse_pos.y)) {
                log_print_n_flush("MAIN!\n");
                }
                else if (rect_point_collision(hitbox.bar, mouse_pos.x, mouse_pos.y)) {
                ctx.held_frame = ctx.active_frame;
                log_print_n_flush("BAR!\n");
                }
                else if (rect_point_collision(hitbox.master, mouse_pos.x, mouse_pos.y)) {
                log_print_n_flush("MASTER!\n");
                }
            } // end hitbox checks.
             

        } // for loop ends

#endif        
       

        if (ctx.held_frame) {
           ctx.held_frame->rect.x += ctx.input_data.mouse_delta_x;
           ctx.held_frame->rect.y += ctx.input_data.mouse_delta_y;
        }

        // frame lag (not sure tho.) but whateever 
        // so here i just translate hitboxes to draw calls no second recalulation of hitboxes.       
        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_RECT;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].rect = hitbox.master;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].color = ctx.theme.color_border;
        ctx.draw_calls.count_rect_pile++;

        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_RECT;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].rect = hitbox.bar;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].color = ctx.theme.color_bar;
        ctx.draw_calls.count_rect_pile++;

        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_RECT;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].rect = hitbox.main;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].color = ctx.theme.color_main;
        ctx.draw_calls.count_rect_pile++;

        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_RECT;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].rect = hitbox.button_close;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].color = ctx.theme.color_button_close;
        ctx.draw_calls.count_rect_pile++;

        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_RECT;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].rect = hitbox.button_zip;
        ctx.draw_calls.rect_pile[ctx.draw_calls.count_rect_pile].color = ctx.theme.color_button_zip;
        ctx.draw_calls.count_rect_pile++;

        ctx.draw_calls.draw_calls_pile[ctx.draw_calls.count++] = EUI_DRAWCALL_TEXT;    
        strncpy(ctx.draw_calls.text_pile[ctx.draw_calls.count_text_pile].text, ctx.active_frame->name, EUI_MAX_TEXT_LEN);
        ctx.draw_calls.text_pile[ctx.draw_calls.count_text_pile].color = ctx.theme.color_text;
        ctx.draw_calls.text_pile[ctx.draw_calls.count_text_pile].font_size = hitbox.text_size;
        ctx.draw_calls.text_pile[ctx.draw_calls.count_text_pile].x = hitbox.text_pos.x;
        ctx.draw_calls.text_pile[ctx.draw_calls.count_text_pile].y = hitbox.text_pos.y;
        ctx.draw_calls.count_text_pile++;

        // DEBUG_UI_BUFFER_UPDATE //
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        char mouse_scroll_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
        snprintf(mouse_scroll_buffer, BUFFER_MAX, "[MOUSE SCROLL: %.2f, %.2f]", mouse_scroll.x, mouse_scroll.y);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            
            int pops_rect = 0;
            int pops_text = 0;
            for (int i = 0; i < ctx.draw_calls.count; i++) {
                assert(pops_rect <= ctx.draw_calls.count_rect_pile);
                assert(pops_text <= ctx.draw_calls.count_text_pile);
                int type = ctx.draw_calls.draw_calls_pile[i];
                switch (type) 
                {
                case EUI_DRAWCALL_RECT:
                DrawRectangle(ctx.draw_calls.rect_pile[pops_rect].rect.x,
                              ctx.draw_calls.rect_pile[pops_rect].rect.y,
                              ctx.draw_calls.rect_pile[pops_rect].rect.width,
                              ctx.draw_calls.rect_pile[pops_rect].rect.height,
                              *(Color*)&ctx.draw_calls.rect_pile[pops_rect].color);
                pops_rect++;
                break;
                case EUI_DRAWCALL_TEXT:
                DrawText(ctx.draw_calls.text_pile[pops_text].text,
                        ctx.draw_calls.text_pile[pops_text].x,
                        ctx.draw_calls.text_pile[pops_text].y,
                        ctx.draw_calls.text_pile[pops_text].font_size,
                        *(Color*)&ctx.draw_calls.text_pile[pops_text].color);
                pops_text++;
                break;
                }
            }            

#if 0       // Debug hitboxes.
            Color DEBUG_COLOR = RED;
            draw_rect_outline(frame.rect, DEBUG_COLOR);
            draw_rect_outline(hitbox.bar, DEBUG_COLOR);
            draw_rect_outline(hitbox.main, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_close, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_zip, DEBUG_COLOR);
            draw_rect_outline(hitbox.button_resize, DEBUG_COLOR);
            DrawText(frame.name, hitbox.text_pos.x, hitbox.text_pos.y, hitbox.text_size, RED);
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


// RenderTexture2D uiTarget = LoadRenderTexture(UI_WIDTH, UI_HEIGHT);
//
// // Render UI to texture
// BeginTextureMode(uiTarget);
// {
//     ClearBackground(BLANK);
//     DrawRectangle(10, 10, 50, 50, DARKGRAY);
//     DrawText("Hello UI", 10, 10, 10, RAYWHITE);
// }
// EndTextureMode();
//
//
// DrawTexturePro(
//        uiTarget.texture,
//        (Rectangle){0, 0, UI_WIDTH, -UI_HEIGHT},  // Source (negative height to flip)
//        (Rectangle){0, 0, screenWidth, screenHeight},  // Destination
//        (Vector2){0, 0},  
//        0.0f,  
//        WHITE
//    );
//
// Setup (once)
// RenderTexture2D target = LoadRenderTexture(800, 600);

// // In your game loop:
// BeginTextureMode(target);
//     ClearBackground(RAYWHITE);
//     DrawRectangle(10, 10, 100, 100, RED);
//     DrawText("Hello", 20, 20, 20, BLACK);
//     // ... all your drawing here
// EndTextureMode();
//
// // Now draw the texture to screen
// BeginDrawing();
//     ClearBackground(BLACK);
//     // Flip texture vertically (OpenGL thing)
//     DrawTextureRec(target.texture, 
//                    (Rectangle){0, 0, target.texture.width, -target.texture.height},
//                    (Vector2){0, 0}, 
//                    WHITE);
// EndDrawing();
//
// Cleanup (when done)
//UnloadRenderTexture(target);

//////////////////////////////////////
// For your fullscreen scaling case //
// // Virtual resolution that gets scaled
// RenderTexture2D target = LoadRenderTexture(800, 600);
//
// while (!WindowShouldClose()) {
//     // Draw to virtual 800x600
//     BeginTextureMode(target);
//         ClearBackground(BLACK);
//         // ... all your UI drawing at 800x600
//     EndTextureMode();
//
//     // Scale to actual screen
//     BeginDrawing();
//         ClearBackground(BLACK);
//         float scale = fminf((float)GetScreenWidth()/800.0f, 
//                            (float)GetScreenHeight()/600.0f);
//         DrawTexturePro(target.texture,
//             (Rectangle){0, 0, 800, -600},  // Source (negative height flips)
//             (Rectangle){0, 0, 800*scale, 600*scale},  // Dest (scaled)
//             (Vector2){0, 0}, 0, WHITE);
//     EndDrawing();
// }

//   // Instead of absolute coords
 //  rect = {100, 100, 200, 50}
   // Use anchors
//   rect = {screenWidth * 0.1, screenHeight * 0.2, 200, 50}
//float uiScale = screenHeight / 1080.0; // designed for 1080p
//rect = {
//    100 * uiScale,
//    100 * uiScale,
//    200 * uiScale,
//    50 * uiScale
//}
