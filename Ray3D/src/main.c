#include "../include/raylib.h"
#include "../include/raymath.h"

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "raylib [models] example - basic voxel");

    DisableCursor();                    // Lock mouse to window center
    

                                        

    // Define the camera to look into our 3d world (first person)
    Camera3D camera = { 0 };
    camera.position = (Vector3){ -2.0f, 0.0f, -2.0f };  // Camera position at ground level
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector
    camera.fovy = 45.0f;                                 // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;              // Camera projection type

    // Create a cube model
    // Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);      // Create a unit cube mesh
    // Model cubeModel = LoadModelFromMesh(cubeMesh);       // Convert mesh to a model
    // cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BEIGE;
    
    Model my_model = LoadModel("models/bizzare.gltf");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawGrid(10, 1.0f);
                DrawModel(my_model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

            EndMode3D();


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(my_model);
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
// //------------------------------------------------------------------------------------
// // Program main entry point
// //------------------------------------------------------------------------------------
// int main(void)
// {
//     // Initialization
//     //--------------------------------------------------------------------------------------
//     const int screenWidth = 800;
//     const int screenHeight = 450;
//
//     InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera free");
//
//     // Define the camera to look into our 3d world
//     Camera3D camera = { 0 };
//     camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
//     camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
//     camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
//     camera.fovy = 45.0f;                                // Camera field-of-view Y
//     camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
//
//     Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
//
//     DisableCursor();                    // Limit cursor to relative movement inside the window
//
//     SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
//     //--------------------------------------------------------------------------------------
//
//     // Main game loop
//     while (!WindowShouldClose())        // Detect window close button or ESC key
//     {
//         // Update
//         //----------------------------------------------------------------------------------
//         UpdateCamera(&camera, CAMERA_FREE);
//
//         if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
//         //----------------------------------------------------------------------------------
//
//         // Draw
//         //----------------------------------------------------------------------------------
//         BeginDrawing();
//
//             ClearBackground(RAYWHITE);
//
//             BeginMode3D(camera);
//
//                 DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
//                 DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
//
//                 DrawGrid(10, 1.0f);
//
//             EndMode3D();
//
//             DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
//             DrawRectangleLines( 10, 10, 320, 93, BLUE);
//
//             DrawText("Free camera default controls:", 20, 20, 10, BLACK);
//             DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
//             DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
//             DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);
//
//         EndDrawing();
//         //----------------------------------------------------------------------------------
//     }
//
//     // De-Initialization
//     //--------------------------------------------------------------------------------------
//     CloseWindow();        // Close window and OpenGL context
//     //--------------------------------------------------------------------------------------
//
//     return 0;
// }
