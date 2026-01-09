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


#include "../include/raylib.h"

int main(void)
{
    InitWindow(800, 600, "TextureRender");
    return 0;
}


