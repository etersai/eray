#include "../include/raylib.h"
#include "../elog.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>

#define local_persist static
#define log_err(msg) do {fprintf(stderr, (msg));} while (0)

#define DEG_TO_RAD (M_PI/180.0)
#define RAD_TO_DEG (180.0/M_PI)

#define max(a, b) ((a)>(b)?(a):(b))

typedef struct {
    float x;
    float y;
} float_v2; 

#define MAP_SIZE (10)
#define TILE_EMPTY (0)
#define TILE_WALL (0)

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

void draw_map(Map map)
{
    if (map.data == NULL) {
        log_err("[Draw_map]: empty map privided, is that what you meant?\n");
        return;
    }
    

        
}

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "raycasting");
    SetTargetFPS(60);

    Map map = map_init(MAP_SIZE, MAP_SIZE);
    if (map.data == NULL) {
        log_err("Fail, cos tam ktos tam!\n");
        exit(1);
    }




    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            draw_map(map);
            DrawFPS(0, 0);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
