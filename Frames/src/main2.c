#include "../include/raylib.h"
#include <stdio.h>
#include <assert.h>

typedef struct { int x, y, width, height; } Rect;
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

Color color_border = {255, 165, 0, 255};
Color color_bar = { 20, 20, 20, 255};
Color color_main = { 50, 50, 50, 255};

// static const e_UI_THEME theme_default = {  // ✅ Even better
//     .default_frame_width = 420,
//     // ...
// };

typedef struct {
    Rect rect;
    bool opened;
    bool zipped_up;
    Rect pre_zipped_dims;
    const char* name; // it's id for now.
    float scale;
} e_UI_FRAME;


static const int BORDER_OFFSET = 2;
static const int ACTION_BAR_SIZE = 16;

static const int FRAME_DEFAULT_WIDTH = 400;
static const int FRAME_DEFAULT_HEIGHT = 300;

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "RewriteKinda");
    SetTargetFPS(60);
        
    Vector2 mouse_pos_prev = {0};  
    int     mouse_delta_x = 0;
    int     mouse_delta_y = 0;
    bool    input_is_left_released = false;
    bool    input_is_left_pressed = false;

    bool held = false;
    Rect master_rect = rect_create(100, 100, FRAME_DEFAULT_WIDTH, FRAME_DEFAULT_HEIGHT);
    while (!WindowShouldClose())
    {
        Vector2 mouse_pos           = GetMousePosition();
        input_is_left_released      = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        input_is_left_pressed       = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
// need mouse scroll.
        mouse_delta_x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta_y = mouse_pos.y - mouse_pos_prev.y;

        if (input_is_left_released) { held = false; }

        if (input_is_left_pressed && rect_point_collision(master_rect, mouse_pos.x, mouse_pos.y)) {
            held = true;
        }
            
        if (held) {
           master_rect.x += mouse_delta_x;
           master_rect.y += mouse_delta_y;
        }

        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);

        // recalcualte hitbox.

        Rect hitbox_master;
        Rect hitbox_action_bar; 
        Rect hitbox_action_button_1;
        Rect hitbox_button_;

        BeginDrawing();
        {


            Color COLOR_BORDER = {};
            Color COLOR_ACTION_BAR = {};
            Color COLOR_MAIN = {};
            Color COLOR_BUTTON_CLOSE = {};
            Color COLOR_BUTTON_ROLL = {};

            ClearBackground(BLACK);
            // BODER as master_rect, serves as it's border and overall hitbox. (masteR_eRect)
            DrawRectangle(master_rect.x,
                          master_rect.y,
                          master_rect.width,
                          master_rect.height,
                          RED);

            // ACTION_BAR
            DrawRectangle(master_rect.x + BORDER_OFFSET, 
                          master_rect.y + BORDER_OFFSET,
                          master_rect.width - (BORDER_OFFSET*2),//(1*2)
                          ACTION_BAR_SIZE,
                          GREEN);
            
            // MAIN AREA
            DrawRectangle(master_rect.x + BORDER_OFFSET,
                          master_rect.y + (2*BORDER_OFFSET) + ACTION_BAR_SIZE,
                          master_rect.width - (2*BORDER_OFFSET),
                          master_rect.height - ((3*BORDER_OFFSET) + ACTION_BAR_SIZE),
                          BLUE);

    
            // Debug.
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
