#include "../include/raylib.h"
#include "../elog.h"
#include <stdio.h>
#include <math.h>

#define local_persist static

#define DEG_TO_RAD (M_PI/180.0)
#define RAD_TO_DEG (180.0/M_PI)

#define max(a, b) ((a)>(b)?(a):(b))

typedef struct {
    float x;
    float y;
} float_v2; 

typedef struct {
    float_v2 pos;
    float angle;
    float fov;
} Playa;

typedef struct {
    float_v2 pos; 
    float_v2 dir; 
} Rayek;

#define DEBUG_DRAW_STARY_X (50)
#define DEBUG_DRAW_STARY_Y (50)
#define DEBUG_CELL_SIZE (50)

#define PLAYER_SPEED_SCALE (0.05f)

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

void draw_debug_view(int draw_start_x, int draw_start_y, Playa player, int* map)
{
    local_persist int cell_draw_size = 50;
    for (int j = 0; j < MAP_SIZE; j++) {
        for (int i = 0; i < MAP_SIZE; i++) {
            Color cell_color; 
            
            if (i == (int)player.pos.x && j == (int)player.pos.y) {
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
    float on_board_player_x = (float)(draw_start_x)+((float)(cell_draw_size) * player.pos.x);
    float on_board_player_y = (float)(draw_start_y)+((float)(cell_draw_size) * player.pos.y);
    DrawCircleV((Vector2){on_board_player_y, on_board_player_x}, 3.5f, GREEN);

    // draw player orientation line.
    local_persist float view_line_scale = 50.0f;
    float line_draw_end_x = cosf(player.angle)*view_line_scale+on_board_player_x;
    float line_draw_end_y = sinf(player.angle)*view_line_scale+on_board_player_y;
    DrawLineV((Vector2){on_board_player_y, on_board_player_x}, (Vector2){line_draw_end_y, line_draw_end_x}, RED);

    float forward_x = cosf(player.angle);
    float forward_y = sinf(player.angle); 
    float fov_half_in_rad = (player.fov/2.0f)*DEG_TO_RAD;

    // TEST
    float step;
    for (step = 0.0; step <= 10.0f; step+=0.01f) {
        float ray_x = player.pos.x+step*forward_x; 
        float ray_y = player.pos.y+step*forward_y; 

        if (map[(int)ray_x * MAP_SIZE + (int)ray_y] == TILE_WALL) {
            break;
        }
    }
    float hit_x = forward_x*step*50.0f+on_board_player_x;
    float hit_y = forward_y*step*50.0f+on_board_player_y;
    DrawCircleV((Vector2){hit_y, hit_x}, 5.0f, RED);
    // END TEST

    float start_fov_x1 = cosf(fov_half_in_rad)*forward_x + forward_y*(-sinf(fov_half_in_rad));
    float start_fov_y1 = sinf(fov_half_in_rad)*forward_x + forward_y*cos(fov_half_in_rad);
    float start_fov_x2 = cosf(-fov_half_in_rad)*forward_x + forward_y*(-sinf(-fov_half_in_rad));
    float start_fov_y2 = sinf(-fov_half_in_rad)*forward_x + forward_y*cos(-fov_half_in_rad);
    float fov_line_end_x1 = start_fov_x1*view_line_scale+on_board_player_x;
    float fov_line_end_y1 = start_fov_y1*view_line_scale+on_board_player_y;
    float fov_line_end_x2 = start_fov_x2*view_line_scale+on_board_player_x;
    float fov_line_end_y2 = start_fov_y2*view_line_scale+on_board_player_y;

    DrawLineV((Vector2){on_board_player_y, on_board_player_x}, (Vector2){fov_line_end_y1, fov_line_end_x1}, PURPLE);
    DrawLineV((Vector2){on_board_player_y, on_board_player_x}, (Vector2){fov_line_end_y2, fov_line_end_x2}, PURPLE);
}




void update_player(Playa* player)
{
    // player forward vector.
    float forward_x = cosf(player->angle);
    float forward_y = sinf(player->angle); 
    float_v2 movement_vector = {0, 0};
    if (IsKeyDown(KEY_W)) {
        movement_vector.x += forward_x;
        movement_vector.y += forward_y;
    }
    if (IsKeyDown(KEY_S)) {
        movement_vector.x -= forward_x;
        movement_vector.y -= forward_y;
    }
    if (IsKeyDown(KEY_A)) {
        movement_vector.x += -forward_y;
        movement_vector.y += forward_x;
    }
    if (IsKeyDown(KEY_D)) {
        movement_vector.x -= -forward_y;
        movement_vector.y -= forward_x;
    }


    if (IsKeyDown(KEY_Q)) {
        player->angle += 0.05f;
    }
    if (IsKeyDown(KEY_E)) {
        player->angle -= 0.05f;
    }

    float movlen = sqrtf(movement_vector.x*movement_vector.x+movement_vector.y*movement_vector.y);
    if (movlen > 0.0f) {
        movement_vector.x = movement_vector.x / movlen;
        movement_vector.y = movement_vector.y / movlen;
    }

    // player forward vector.
    float new_player_x = player->pos.x+(movement_vector.x*PLAYER_SPEED_SCALE);
    float new_player_y = player->pos.y+(movement_vector.y*PLAYER_SPEED_SCALE);
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

    Playa player = {0};
    player.pos.x = 4.23f;
    player.pos.y = 3.24f;
    player.angle = 0.0f;
    player.fov = 70.0f;
    
    bool debug_view = true; 
    while (!WindowShouldClose())
    {
        update_player(&player);
        if (IsKeyPressed(KEY_GRAVE)) {
            debug_view = !debug_view; 
        }

        float angle_step_size = player.fov / screenWidth;
        float forward_x = cosf(player.angle);
        float forward_y = sinf(player.angle); 
        float fov_half_in_rad = (player.fov/2.0f)*DEG_TO_RAD;
        float start_fov_x1 = cosf(fov_half_in_rad)*forward_x + forward_y*(-sinf(fov_half_in_rad));
        float start_fov_y1 = sinf(fov_half_in_rad)*forward_x + forward_y*cos(fov_half_in_rad);
        float accumulated_angle = 0.0f;
        while (accumulated_angle < player.fov) {

            float step;
            for (step = 0.0; step <= 10.0f; step+=0.01f) {
                float ray_x = player.pos.x+step*forward_x; 
                float ray_y = player.pos.y+step*forward_y; 

                if (map[(int)ray_y][(int)ray_x] == TILE_WALL) {
                    break;
                }
            }              


            start_fov_x1 = cosf(-angle_step_size)*forward_x + forward_y*(-sinf(-angle_step_size));
            start_fov_y1 = sinf(-angle_step_size)*forward_x + forward_y*cos(-angle_step_size);
            accumulated_angle+=angle_step_size; 
        }

                

        BeginDrawing();
            ClearBackground(BLACK);
            if (debug_view) {
                draw_debug_view(DEBUG_DRAW_STARY_X, DEBUG_DRAW_STARY_Y, player, (int*)map);
                DrawFPS(0, 0);
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
