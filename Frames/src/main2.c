#include "../include/raylib.h"
#include <stdio.h>
#include <assert.h>

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

static const int DEFAULT_BORDER_OFFSET = 1;
static const int DEFAULT_BAR_HEIGHT = 32;
static const int DEFAULT_FRAME_WIDTH = 420;
static const int DEFAULT_FRAME_HEIGHT = 300;
static const int DEFAULT_BUTTON_SIZE = 16; 

typedef struct {
    e_UI_COLOR color;
    Rect       rect; // preferably 4 ints but no time for that XD. (NOTE: will regret it XD)
} eui_DrawRectInfo;

typedef struct {
    Rect rect;
    Rect rect_prev;
    bool opened;
    bool zip;
    const char* name; // it's id for now.
    float scale; // 1.0f default.
} e_UI_FRAME;



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
    Rect master_rect = rect_create(800, 400, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT);
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

        // DEBUG_UI_BUFFER_UPDATE //
        enum { BUFFER_MAX = 64 };
        char mouse_pos_buffer[BUFFER_MAX];
        char mouse_delta_buffer[BUFFER_MAX];
        snprintf(mouse_pos_buffer, BUFFER_MAX, "[MOUSE POS: %.2f, %.2f]", mouse_pos.x, mouse_pos.y);
        snprintf(mouse_delta_buffer, BUFFER_MAX, "[MOUSE DELTA: %d, %d]", mouse_delta_x, mouse_delta_y);
    
        // RECT RECALCULATIONS //
        Rect hitbox_master;
        Rect hitbox_bar; // |-| .,, |& . In memory of our pullup BAR.
        Rect hitbox_main;
        // clickables.
        Rect hitbox_button_close;
        Rect hitbox_button_zip;
        Rect hitbox_button_resize;
      
        static bool zip = false;
        static float SCALE = 1.0f;
        
        SCALE+=0.01;
        if (SCALE >= 2.0f) {
            SCALE = 1.0f;
        }

        hitbox_master.x = master_rect.x;
        hitbox_master.y = master_rect.y;
        hitbox_master.width = DEFAULT_FRAME_WIDTH * SCALE;
        if (zip) { hitbox_master.height = DEFAULT_BAR_HEIGHT*SCALE + 2*DEFAULT_BORDER_OFFSET; }
        else { hitbox_master.height = DEFAULT_FRAME_HEIGHT*SCALE; }

        hitbox_bar.x = hitbox_master.x + DEFAULT_BORDER_OFFSET; 
        hitbox_bar.y = hitbox_master.y + DEFAULT_BORDER_OFFSET;
        hitbox_bar.width = hitbox_master.width - DEFAULT_BORDER_OFFSET*2;
        hitbox_bar.height = DEFAULT_BAR_HEIGHT * SCALE;
    
        if (zip) {
            Rect NULL_HITBOX = {0};
            hitbox_main = NULL_HITBOX;
        }
        else
        {
            hitbox_main.x = hitbox_master.x + DEFAULT_BORDER_OFFSET; 
            hitbox_main.y = hitbox_master.y + 2*DEFAULT_BORDER_OFFSET + hitbox_bar.height;
            hitbox_main.width = hitbox_master.width - 2*DEFAULT_BORDER_OFFSET;
            hitbox_main.height = hitbox_master.height + hitbox_bar.height - 3*DEFAULT_BORDER_OFFSET;
        }

        log_rect_print_n_flush(hitbox_main);

        // BUTTONS //* */
        int button_slot_y_axis = hitbox_master.y + hitbox_bar.height/2 + DEFAULT_BORDER_OFFSET;
        int button_slot_0_x = hitbox_master.x + hitbox_master.width - (DEFAULT_BUTTON_SIZE*SCALE)+DEFAULT_BORDER_OFFSET;
        int button_slot_1_x = hitbox_master.x + hitbox_master.width - 2*((DEFAULT_BUTTON_SIZE*SCALE)+DEFAULT_BORDER_OFFSET);

        // close
        hitbox_button_close.x = button_slot_0_x - (DEFAULT_BUTTON_SIZE*SCALE/2);            
        hitbox_button_close.y = button_slot_y_axis - (DEFAULT_BUTTON_SIZE*SCALE/2);             
        hitbox_button_close.width = DEFAULT_BUTTON_SIZE * SCALE;            
        hitbox_button_close.height = DEFAULT_BUTTON_SIZE * SCALE;            
        
        // zip
        hitbox_button_zip.x = button_slot_1_x - DEFAULT_BUTTON_SIZE*SCALE;            
        hitbox_button_zip.y = button_slot_y_axis - (DEFAULT_BUTTON_SIZE*SCALE/2);             
        hitbox_button_zip.width = DEFAULT_BUTTON_SIZE * SCALE;            
        hitbox_button_zip.height = DEFAULT_BUTTON_SIZE * SCALE;            

        // ONLY AFTER RECALCULATIONS THIS NEW MAIN HITBOX BECOMES SAVED AS STRUCT STATE.
        // recalculated becomes the master.
        master_rect = hitbox_master;

        // if zipped_up dont send playground draw.
        // Make draw command.

        BeginDrawing();
        {
            ClearBackground(BLACK);
            // BODER as master_rect, serves as it's border and overall hitbox. (masteR_eRect)
#if 0
            DrawRectangle(master_rect.x,
                          master_rect.y,
                          master_rect.width,
                          master_rect.height,
                          *(Color*)&theme_default.color_border);

            // BAR
            DrawRectangle(hitbox_bar.x,
                        hitbox_bar.y,
                        hitbox_bar.width,
                        hitbox_bar.height,
                        *(Color*)&theme_default.color_bar); 

            // MAIN AREA
            DrawRectangle(hitbox_main.x,
                          hitbox_main.y,
                          hitbox_main.width,
                          hitbox_main.height,
                          *(Color*)&theme_default.color_main);

            // NOW BUTTONS
            DrawRectangle(hitbox_button_close.x,
                    hitbox_button_close.y,
                    hitbox_button_close.width,
                    hitbox_button_close.height,
                    *(Color*)&theme_default.color_button_close);

            DrawRectangle(hitbox_button_zip.x,
                    hitbox_button_zip.y,
                    hitbox_button_zip.width,
                    hitbox_button_zip.height,
                    *(Color*)&theme_default.color_button_zip);
#endif 
#if 1       // Debug hitboxes.
            Color DEBUG_COLOR = RED;
            draw_rect_outline(master_rect, DEBUG_COLOR);
            draw_rect_outline(hitbox_bar, DEBUG_COLOR);
            draw_rect_outline(hitbox_main, DEBUG_COLOR);
            draw_rect_outline(hitbox_button_close, DEBUG_COLOR);
            draw_rect_outline(hitbox_button_zip, DEBUG_COLOR);
#endif
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
