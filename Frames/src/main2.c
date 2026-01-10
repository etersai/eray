#include "../include/raylib.h"
#define UI_WIDTH 160
#define UI_HEIGHT 90

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "UFF TESTY");
    SetTargetFPS(60);
    
    RenderTexture2D uiTarget = LoadRenderTexture(UI_WIDTH, UI_HEIGHT);
    
    while (!WindowShouldClose())
    {
        // Render UI to texture
        BeginTextureMode(uiTarget);
        {
            ClearBackground(BLANK);
            DrawRectangle(10, 10, 50, 50, DARKGRAY);
            DrawText("Hello UI", 10, 10, 10, RAYWHITE);
        }
        EndTextureMode();
        
        // Render to screen
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            
            DrawTexturePro(
                uiTarget.texture,
                (Rectangle){0, 0, UI_WIDTH, -UI_HEIGHT},  // Source (negative height to flip)
                (Rectangle){0, 0, screenWidth, screenHeight},  // Destination
                (Vector2){0, 0},  
                0.0f,  
                WHITE
            );
            
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        }
        EndDrawing();
    }
    
    UnloadRenderTexture(uiTarget);
    CloseWindow();
    
    return 0;
}

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
