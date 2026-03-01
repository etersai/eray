#include "../include/raylib.h"
#include "../elog.h"
#include <stdio.h>
#include <math.h>

#define local_persist static

typedef struct {
    float x;
    float y;
} float_v2; 

typedef struct {
    float_v2 pos;
    float angle;
    float fov;
} Playa;

#define DEBUG_DRAW_STARY_X (50)
#define DEBUG_DRAW_STARY_Y (50)
#define DEBUG_CELL_SIZE (50)

#define TILE_WALL (1)
#define TILE_EMPTY (0)
#define MAP_SIZE (10)
int map[MAP_SIZE][MAP_SIZE] = 
{
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,1,1},
    {1,0,1,1,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1},
};

void draw_debug_cells(int draw_start_x, int draw_start_y, float player_angle, float player_pos_x, float player_pos_y, int* map)
{
    local_persist int cell_draw_size = 50;
    for (int j = 0; j < MAP_SIZE; j++) {
        for (int i = 0; i < MAP_SIZE; i++) {
            Color cell_color; 
            
            if (i == (int)player_pos_x && j == (int)player_pos_y) {
                cell_color = DARKGRAY;
            }

            else if (map[i*MAP_SIZE+j] == TILE_WALL) {
                cell_color = ORANGE; 
            }
            else // TILE_EMPTY
            {
                cell_color = GRAY;
            }

            DrawRectangle((j*cell_draw_size)+draw_start_x, (i*cell_draw_size)+draw_start_y, cell_draw_size-1, cell_draw_size-1, cell_color); 
        }
    }

    // draw actuall player pos.
    float on_board_player_x = (float)(draw_start_x)+((float)(cell_draw_size) * player_pos_x);
    float on_board_player_y = (float)(draw_start_y)+((float)(cell_draw_size) * player_pos_y);
    DrawCircleV((Vector2){on_board_player_y, on_board_player_x}, 3.5f, GREEN);

    // draw player orientation line.
    float line_draw_start_x = on_board_player_x;
    float line_draw_start_y = on_board_player_y;
    float line_draw_end_x = cosf(player_angle)*50.0f+on_board_player_x;
    float line_draw_end_y = sinf(player_angle)*50.0f+on_board_player_y;
    DrawLineV((Vector2){line_draw_start_y, line_draw_start_x}, (Vector2){line_draw_end_y, line_draw_end_x}, RED);


}


void update_player(Playa* player)
{
    float_v2 movement_vector = {0};
    if (IsKeyDown(KEY_W)) {
        movement_vector.x -= 1.0f;
    }
    if (IsKeyDown(KEY_S)) {
        movement_vector.x += 1.0f;
    }
    if (IsKeyDown(KEY_A)) {
        movement_vector.y -= 1.0f;
    }
    if (IsKeyDown(KEY_D)) {
        movement_vector.y += 1.0f;
    }

    if (IsKeyDown(KEY_Q)) {
        player->angle += 0.05f;
    }
    if (IsKeyDown(KEY_E)) {
        player->angle -= 0.05f;
    }

    float movlen = sqrtf(movement_vector.x*movement_vector.x+movement_vector.y*movement_vector.y);
    if (movlen > 0.0) {
        movement_vector.x = movement_vector.x / movlen;
        movement_vector.y = movement_vector.y / movlen;
    }

    local_persist float speed_scale_factor = 0.05f;

    float new_player_x = player->pos.x+(movement_vector.x*speed_scale_factor);
    float new_player_y = player->pos.y+(movement_vector.y*speed_scale_factor);

    int to_be_cell_x = (int)(new_player_x);
    int to_be_cell_y = (int)(new_player_y);

    if (map[to_be_cell_x][to_be_cell_y] == TILE_EMPTY) {
        player->pos.x = new_player_x;
        player->pos.y = new_player_y;
    }
}



int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "raycasting");
    SetTargetFPS(60);

#if 0 
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            printf("%d", map[i][j]);
        }
        putchar('\n');
    }
#endif

    Playa player = {0};
    player.pos.x = 4.23f;
    player.pos.y = 3.24f;
    bool debug_view = true; 
    while (!WindowShouldClose())
    {
        update_player(&player);
        if (IsKeyPressed(KEY_GRAVE)) {
        
        }

        BeginDrawing();
            ClearBackground(BLACK);
            if (debug_view) {
                draw_debug_cells(DEBUG_DRAW_STARY_X, DEBUG_DRAW_STARY_Y, player.angle, player.pos.x, player.pos.y, (int*)map);
                DrawFPS(0, 0);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
