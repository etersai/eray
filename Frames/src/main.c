#include "../include/raylib.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>

//
// LOG 
//
#define LOG_BUFFER_BEAUTIFY_MAX 64
void log_print_beautify(const char* prefix, const char* format, ...)
{
    int idx = 0;
    char buffer[LOG_BUFFER_BEAUTIFY_MAX] = {0};
    while (*prefix) {
        if (idx >= LOG_BUFFER_BEAUTIFY_MAX) {
            break; 
        }
        buffer[idx] = *prefix;
        prefix++; idx++;
    } // TODO: right now it does nothing. but expand.
    
    fprintf(stderr, "[%s]: ", buffer);
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Use vprintf for va_list
    va_end(args);
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
// UI RECT
//
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect;

Rect rect_create(int x, int y, int width, int height);
static inline void rect_update_pos(Rect* r, int x, int y);
static inline void rect_update_pos(Rect* r, int x, int y);
static inline void rect_update_dims(Rect* r, int width, int height);
static inline void rect_update_width(Rect* r, int width);
static inline void rect_update_height(Rect* r, int height);
static inline bool rect_aabb_collision(Rect r1, Rect r2);
static inline bool rect_point_collision(Rect r, int x, int y);
void rect_printf(Rect rect);

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
}    bool IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
    bool IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
    bool IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
    bool IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed

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

static inline bool rect_point_collision(Rect r, int x, int y)
{
    return rect_aabb_collision(r, (Rect){x, y, 1, 1});
}

static inline bool rect_aabb_collision(Rect r1, Rect r2)
{
    return (r1.x + r1.width >= r2.x && r1.x <= r2.x + r2.width        // X_AXIS
            && r1.y + r1.height >= r2.y && r1.y <= r2.y + r2.height); // Y_AXIS
}

void rect_printf(Rect rect)
{
    printf("[x:%d, y:%d, w:%d, h:%d]\n", rect.x, rect.y, rect.width, rect.height);
}

//
// UI MEAT
//
typedef enum {
    EUI_MOUSE_LEFT  = (1 << 0),
    EUI_MOUSE_RIGHT = (1 << 1),
} eui_mouse_button;

typedef enum {
    EUI_WINDOW_NAILED = (1 << 0),
} eui_window_option; 


#define EUI_FRAMES_MAX 64
typedef struct {
    Rect  rect;
    Color color;
    bool in_air;
    bool hovered;
} e_UI_FRAME;

e_UI_FRAME FRAMES[EUI_FRAMES_MAX];

typedef struct {
    int mpos_x;
    int mpos_y;
} e_UI;

void eui_input_mousepos(e_UI* eui, int mpos_x, int mpos_y);
void eui_input_mousedown(e_UI* eui, int mpos_x, int mpos_y, eui_mouse_button btn);
void eui_input_mouseup(e_UI* eui, int mpos_x, int mpos_y, eui_mouse_button btn);

bool eui_window_make(e_UI* eui, Rect r, const char* label, uint32_t opt);
bool eui_button(e_UI* eui, const char* label);

void eui_input_mousepos(e_UI* eui, int mpos_x, int mpos_y)
{
    eui->mpos_x = mpos_x;
    eui->mpos_y = mpos_y;
}

void eui_input_mousedown(e_UI* eui, int mpos_x, int mpos_y, eui_mouse_button btn)
{
     
}

void eui_input_mouseup(e_UI* eui, int mpos_x, int mpos_y, eui_mouse_button btn)
{

}

int main(void)
{
    int width = 1280;
    int height = 720;
    InitWindow(width, height, "Rets");
    SetTargetFPS(60);
    int mouse_delta_x = 0;
    int mouse_delta_y = 0;

    // passing by value for now.
    size_t frames_count = 0;
    e_UI_FRAME frame = {0};
    frame.rect = rect_create(600, 300, 300, 100);
    frame.color = ORANGE;
    FRAMES[frames_count] = frame;
    frames_count++;
    log_print_beautify("FRAME_COUNT", "%d\n", frames_count);

    while (!WindowShouldClose()) {

        Vector2 mouse_pos = GetMousePosition();
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "[%.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        
        // Reset user input.
        // bool IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
        // bool IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
        // bool IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
        // bool IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
        for (int i = 0; i < frames_count; i++) {

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                FRAMES[i].in_air = false;
            }
        

            FRAMES[i].hovered = false;
        }
               
        // Update state.
        for (int i = 0; i < frames_count; i++) {
            
            if (rect_point_collision(FRAMES[i].rect, mouse_pos.x, mouse_pos.y)) {
                FRAMES[i].hovered = true;

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    FRAMES[i].in_air = true;
                }
            }

            if (FRAMES[i].in_air) {
                Rect rect = FRAMES[i].rect;
                // PEN AND PAPER HERE.
                rect.x;

            }

        } 

        BeginDrawing();
            {
                ClearBackground(DARKGRAY);
               
                for (int i = 0; i < frames_count; i++) {
                    // Draw frame.
                    Rect rect = FRAMES[i].rect;
                    Color final;
                    float fade_level = 0.9f;
                    if (FRAMES[i].hovered) {
                        final = Fade(FRAMES[i].color, fade_level);
                    } else {
                        final = FRAMES[i].color;
                    }
                    DrawRectangle(rect.x, rect.y, rect.width, rect.height,final);
                }

                // UI
                DrawText(buffer, 0, 15, 20, BLACK);
                DrawFPS(0, 0);
            }
        EndDrawing();

        Vector2 mouse_pos_last = GetMousePosition();

        int mouse_delta_x = mouse_pos_last.x - mouse_pos.x;
        int mouse_delta_y = mouse_pos_last.y - mouse_pos.y;
        // raylib opt (Raysan GOAT).
        Vector2 md= GetMouseDelta(); 
        printf("MY:[%d, %d] RAY:[%f, %f]\n", mouse_delta_x, mouse_delta_y, md.x, md.y);
        fflush(stdout);
    }

    CloseWindow();
    return 0;
}

// OLD SHAISE
// int canvas_width   = 1280;
// int canvas_height  = 720;
//
// void screen_to_center_origin(int pos_x, int pos_y, int* out_x, int* out_y)
// {
//     int rx = pos_x - canvas_width/2;
//     int ry = (pos_y - canvas_height/2) * -1;
//
//     *out_x = rx;
//     *out_y = ry;
// }
//
// void center_origin_to_screen(int pos_x, int pos_y, int* out_x, int* out_y)
// {
//     int result_x = 0;
//     int result_y = 0;
//
//     // from a [top left 0|0] to b [origin 640|360] = [B - A]
//     int atobx = canvas_width/2;  // 640
//     int atoby = canvas_height/2; // 360
//
//     result_x = atobx + pos_x;
//     result_y = atoby - pos_y;
//
//     *out_x = result_x;
//     *out_y = result_y;
// }
//
// void PutPixelCenterOrigin(int pos_x, int pos_y, const Color color)
// {
//     int rx, ry;
//     center_origin_to_screen(pos_x, pos_y, &rx, &ry);
//     DrawPixel(rx, ry, color);
// }
//
// void CanvasToViewport(int cx, int cy, vec3 vp)
// {
//     vp[0] = cx * (1.0f/canvas_width);
//     vp[1] = cy * (1.0f/canvas_height);
//     vp[2] = 1.0f;
// }
//
// void calculate_ray(vec2 P, const vec2 Origin, const vec2 dir, float t)
// { // P = O + t(V-O) 
//     P[0] = Origin[0] + dir[0] * t;
//     P[1] = Origin[1] + dir[1] * t;
// }
//
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
