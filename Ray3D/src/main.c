#include "../include/raylib.h"
#include "../elog.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>

#define local_persist static
#define global        static
#define log_err(msg) do {fprintf(stderr, (msg));} while (0)

#define DEG_TO_RAD (M_PI/180.0)
#define RAD_TO_DEG (180.0/M_PI)
#define max(a, b) ((a)>(b)?(a):(b))

typedef struct {
    float x;
    float y;
} float_v2; 

typedef struct {
    float_v2 origin;
    float_v2 dir;
} float_ray_2d;

#define TILE_EMPTY (0)
#define TILE_WALL (1)

typedef struct {
    int*   data;
    size_t cols; // x ->
    size_t rows; // y \/
} Map;

Map map_init(size_t cols, size_t rows)
{
    assert(cols > 0);
    assert(rows > 0);
    Map map = {0};
    map.data = calloc(1, cols*rows*sizeof(*map.data));
    if (map.data == NULL) {
        return map;
    }
    map.cols = cols;
    map.rows = rows;
    return map;
}

global int draw_tile_size = 50;
global int draw_map_offset = 20;
void draw_map(Map map)
{
    if (map.data == NULL) {
        log_err("[draw_map]: Empty map provided!\n");
        return;
    }
    
    for (int y = 0; y < map.rows; y++) {
        for (int x = 0; x < map.cols; x++) {
            Color tile_color; 
            int tile = map.data[y*map.cols+x];
            if (tile == TILE_WALL) {
                tile_color = DARKBROWN;
            }
            else
            {
                tile_color = DARKGRAY;
            }

            DrawRectangle((x*draw_tile_size)+draw_map_offset, (y*draw_tile_size)+draw_map_offset,
                    draw_tile_size-1, draw_tile_size-1, tile_color);
        }
    }
}

float_v2 get_mouse_pos(void)
{
    Vector2 mp = GetMousePosition();
    return (float_v2){mp.x, mp.y};
}

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "raycasting");
    SetTargetFPS(60);

    Map map = map_init(10, 10);
    if (map.data == NULL) {
        log_err("Fail, cos tam ktos tam!\n");
        exit(1);
    }

    float_v2 anchor_map_space = {5.0f, 5.0f};
    float_v2 anchor_tile_space = {anchor_map_space.x*draw_tile_size+draw_map_offset,
                                  anchor_map_space.y*draw_tile_size+draw_map_offset}; 


    while (!WindowShouldClose())
    {
        float_v2 mouse_pos = get_mouse_pos();
        float map_space_mx = (mouse_pos.x/draw_tile_size)-((float)draw_map_offset/draw_tile_size); 
        float map_space_my = (mouse_pos.y/draw_tile_size)-((float)draw_map_offset/draw_tile_size); 
        float angle = atan2((anchor_map_space.y-map_space_my), map_space_mx-anchor_map_space.x);
    
        float_ray_2d ray = {0};
        ray.origin.x = anchor_map_space.x;
        ray.origin.y = anchor_map_space.y; 
        ray.dir.x = cosf(angle);
        ray.dir.y = sinf(-angle);

#if 0
        elog_s("******");
        elog_f(ray.origin.x);
        elog_f(ray.origin.y);
        elog_f(ray.dir.x);
        elog_f(ray.dir.y);
        elog_s("******");
#endif

        float ray_scale_x = sqrt(1+(ray.dir.y/ray.dir.x)*(ray.dir.y/ray.dir.x));
        float ray_scale_y = sqrt(1+(ray.dir.x/ray.dir.y)*(ray.dir.x/ray.dir.y));

        BeginDrawing();
            ClearBackground(BLACK);
            draw_map(map);

            DrawCircleV(*(Vector2*)&anchor_tile_space, 5.0f, RED);

            float dot_x = 0.0f;
            float dot_y = 0.0f;
    
            float scaled_ray_gs_x = 0.0f;
            float scaled_ray_gs_y = 0.0f;
            if (ray_scale_y > ray_scale_x) {
                float scaled_ray_x = ray.origin.x+ray.dir.x*ray_scale_x;
                float scaled_ray_y = ray.origin.y+ray.dir.y*ray_scale_x;

                scaled_ray_gs_x = scaled_ray_x*draw_tile_size+draw_map_offset;
                scaled_ray_gs_y = scaled_ray_y*draw_tile_size+draw_map_offset;
            }
            else
            {
                float scaled_ray_x = ray.origin.x+ray.dir.x*ray_scale_y;
                float scaled_ray_y = ray.origin.y+ray.dir.y*ray_scale_y;

                scaled_ray_gs_x = scaled_ray_x*draw_tile_size+draw_map_offset;
                scaled_ray_gs_y = scaled_ray_y*draw_tile_size+draw_map_offset;
            }

            DrawLineV((Vector2){anchor_tile_space.x, anchor_tile_space.y}, (Vector2){scaled_ray_gs_x, scaled_ray_gs_y}, RED);

            // dot to grid space
            dot_x = dot_x*draw_tile_size+draw_map_offset;
            dot_y = dot_y*draw_tile_size+draw_map_offset;

            elog_f(dot_x);
            elog_f(dot_y);

            DrawCircleV((Vector2){dot_x, dot_y}, 5.0f, ORANGE);

            DrawFPS(0, 0);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
