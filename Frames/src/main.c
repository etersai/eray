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
typedef struct e_UI_FRAME_ELEMENT e_UI_FRAME_ELEMENT;
typedef struct e_UI_FRAME e_UI_FRAME;

typedef struct {
     Color color;
     Rect  rect;   
} Piece;

struct e_UI_FRAME {
    Rect         rect;
    Color        color;
    unsigned int id;
    bool         in_air;
    bool         hovered;
    
    e_UI_FRAME_ELEMENT *elements;
    size_t              element_count;
    size_t              element_capacity;
}; /* e_UI_FRAME */

struct e_UI_FRAME_ELEMENT {
    Piece      *pieces; 
    size_t      pieces_count;
    e_UI_FRAME *owner;
}; /* e_UI_FRAME_ELEMENT */

e_UI_FRAME FRAMES[EUI_FRAMES_MAX];

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

    // Create frame (passing by value for now.)
    int frame_count = 0;
    e_UI_FRAME frame = {0};
    frame.rect = rect_create(600, 300, 300, 100);
    frame.color = ORANGE;
    frame.id = frame_count;

    e_UI_FRAME_ELEMENT element_border;
    element_border.owner = &frame;
    Piece piece;
    piece.color = GRAY;
    enum { ELEMENT_BORDER_SIZE = 4 };
    piece.rect.x = element_border.owner->rect.x + ELEMENT_BORDER_SIZE;
    piece.rect.y = element_border.owner->rect.y + ELEMENT_BORDER_SIZE;
    piece.rect.width = element_border.owner->rect.width - (2*ELEMENT_BORDER_SIZE); 
    piece.rect.height = element_border.owner->rect.height - (2*ELEMENT_BORDER_SIZE);

    element_border.pieces = &piece; 
    element_border.pieces_count = 1;

    frame.elements = &element_border;
    frame.element_count++;      

    FRAMES[frame_count] = frame;
    frame_count++;

    log_print_beautify("frame_count", "%d\n", frame_count);
    
    Vector2 mouse_pos_prev = {0};  
    bool input_is_left_released = false;
    while (!WindowShouldClose()) {

        Vector2 mouse_pos = GetMousePosition();
        input_is_left_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;

        // Cheatsheet //     
        // bool IsMouseButtonPressed(int button);  // Check if a mouse button has been pressed once
        // bool IsMouseButtonDown(int button);     // Check if a mouse button is being pressed
        // bool IsMouseButtonReleased(int button); // Check if a mouse button has been released once
        // bool IsMouseButtonUp(int button);       // Check if a mouse button is NOT being pressed
        
        // Reset state.
        for (int i = 0; i < frame_count; i++) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !input_is_left_released) {
                FRAMES[i].hovered = false;
            }
            else
            {
                FRAMES[i].in_air = false;
                FRAMES[i].hovered = false;
            }
        }
               
        // Update state.
        for (int i = 0; i < frame_count; i++) {
            
            if (rect_point_collision(FRAMES[i].rect, mouse_pos.x, mouse_pos.y)) {
                FRAMES[i].hovered = true;

            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && FRAMES[i].hovered) {
                 FRAMES[i].in_air = true;
            }   

            if (FRAMES[i].in_air) {
                // think about whats the way to access elements to be both readable and fast.
                FRAMES[i].rect.x += mouse_delta_x;
                FRAMES[i].rect.y += mouse_delta_y;
                FRAMES[i].elements->pieces->rect.x = FRAMES[i].rect.x + ELEMENT_BORDER_SIZE;
                FRAMES[i].elements->pieces->rect.y = FRAMES[i].rect.y + ELEMENT_BORDER_SIZE;            
            }
        } 
   
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        char frame_title_buffer[BUFFER_MAX];
        char frame_rect_buffer[BUFFER_MAX];
        char frame_hovered_buffer[BUFFER_MAX];
        char frame_in_air_buffer[BUFFER_MAX];

        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
    
        e_UI_FRAME frame = FRAMES[0];
        snprintf(frame_title_buffer, BUFFER_MAX, "[FRAME ID: %d]", frame.id);
        snprintf(frame_rect_buffer, BUFFER_MAX, "[RECT: x:%d y:%d w:%d h:%d]", 
                frame.rect.x, frame.rect.y, frame.rect.width, frame.rect.height);
        snprintf(frame_hovered_buffer, BUFFER_MAX, "[HOVERED: %s]", stringify_bool(frame.hovered));
        snprintf(frame_in_air_buffer, BUFFER_MAX, "[AIRED: %s]", stringify_bool(frame.in_air));

        BeginDrawing();
            {
                ClearBackground(BLACK);
                
                // Draw Frames
                for (int curr_frame = 0; curr_frame < frame_count; curr_frame++) {
                    // Rect rect = FRAMES[i].rect;
                    // Color final;
                    // float fade_level = 0.9f;
                    // if (FRAMES[i].hovered) {
                    //     final = Fade(FRAMES[i].color, fade_level);
                    // }
                    // else
                    // {
                    //     final = FRAMES[i].color;
                    // }
                    e_UI_FRAME frame = FRAMES[curr_frame];
                    DrawRectangle(frame.rect.x, frame.rect.y, frame.rect.width, frame.rect.height, frame.color);
                    for (int i = 0; i < frame.element_count; i++) {

                        if (frame.elements == NULL) { // hackyyy
                            log_print_n_flush("NULL!");
                            abort();
                        }

                        for (int j = 0; j < frame.elements->pieces_count; j++) {
                            DrawRectangle(frame.elements->pieces[i].rect.x,
                                            frame.elements->pieces[i].rect.y,
                                            frame.elements->pieces[i].rect.width,
                                            frame.elements->pieces[i].rect.height,
                                            frame.elements->pieces[i].color);
                        }
                    }

                    
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

            }
        EndDrawing();
        mouse_pos_prev = mouse_pos;
    }

    CloseWindow();
    return 0;
}

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
