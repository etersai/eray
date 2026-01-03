#include <SDL3/SDL.h>

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define PI 3.14159265358979323846

#define ETER_MIN(a,b) ((a) < (b) ? (a) : (b))
#define ETER_MAX(a,b) ((a) > (b) ? (a) : (b))
#define ETER_ABS(x) ((x) < 0 ? -(x) : (x))
#define ARRLEN(arr) (sizeof(arr)/sizeof(arr[0]))
#define UNUSED(var) (void)(var)

uint32_t get_random_bits_uint32_t(void)
{ // 0x00007fff masks first 15 bits;
   uint32_t r = 0;                // it looks like a slot machine :D.
    r |= ((uint32_t)(rand() & 0x00007fff) << 17);
    r |= ((uint32_t)(rand() & 0x00007fff) << 2);
    r |= ((uint32_t)(rand() & 0x00000003));
    return r;
}

// NOTE: Color format assumes little-endian architecture (x86, ARM) 
#define ERAY_COLOR_RED       (0xff0000ff) 
#define ERAY_COLOR_GREEN     (0xff00ff00) 
#define ERAY_COLOR_BLUE      (0xffff0000)
#define ERAY_COLOR_WHITE     (0xffffffff)
#define ERAY_COLOR_BLACK     (0xff000000)
#define ERAY_COLOR_GRAY      (0xff808080)
#define ERAY_COLOR_DARKGRAY  (0xff404040)
#define ERAY_COLOR_LIGHTGRAY (0xffc0c0c0)

typedef struct {
    uint32_t *pixels;
    size_t width;
    size_t height;
    size_t stride;
} Eray_Canvas;

#define ERAY_PIXEL(ec, x, y)   (ec).pixels[(y)*(ec).width + (x)] 

Eray_Canvas eray_init(uint32_t *pixels, size_t width, size_t height, size_t stride)
{
    Eray_Canvas ec = {
        .pixels = pixels,
        .width = width,
        .height = height,
        .stride = stride,
    };
    return ec;
}


void eray_fill(Eray_Canvas ec, uint32_t color)
{
    size_t px_count = ec.width*ec.height;
    for (size_t i = 0; i < px_count; i++) {
        ec.pixels[i]= color;
    } 
}

void eray_checkerboard(Eray_Canvas ec, uint32_t c1, uint32_t c2, size_t cell_size)
{
    for (size_t i = 0; i < ec.width; i++) {
        for (size_t j = 0; j < ec.height; j++) { 
            int cell_y = j / cell_size;
            int cell_x = i / cell_size;
            uint32_t color = (cell_x + cell_y) % 2 == 0 ? c1 : c2; 
            ERAY_PIXEL(ec, i, j) = color;
        }
    }
}

float eray_aspect_ratio(Eray_Canvas ec)
{
    return (float)ec.width / ec.height;
}

void eray_put_pixel(Eray_Canvas ec, int x, int y, uint32_t color)
{
    ERAY_PIXEL(ec, x, y) = color;
}

void eray_plot_line(Eray_Canvas ec, int x1, int y1, int x2, int y2, uint32_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        eray_put_pixel(ec, x1, y1, color);
        return;
    }

    // y = y1 + m(x - x1). point-slope form.
    if (ETER_ABS(dx) >= ETER_ABS(dy)) { // step X.
    
        int step = dx < 0 ? -1 : 1;
        int x_start = x1;
        int y_start = y1;
        float m = (float)dy/dx;
        
        for (int i = 0; i < ETER_ABS(dx); i++) {
            eray_put_pixel(ec, x1, y1, color);
            x1+=step;
            y1 =y_start + m * (x1 - x_start); 
        }

    }
    else // step Y.
    { 
        int step = dy < 0 ? -1 : 1;
        int x_start = x1;
        int y_start = y1;
        float m = (float)dy/dx;
        
        for (int i = 0; i < ETER_ABS(dy); i++) {
            eray_put_pixel(ec, x1, y1, color);
            y1+=step;
            x1 =x_start + (y1 - y_start) / m; 
        }
    }
}

#define CANVAS_WIDTH 1280
#define CANVAS_HEIGHT 720
uint32_t pixels[CANVAS_WIDTH*CANVAS_HEIGHT];
static float cube[24] = {
    1.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 1.0f,
   -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
   -1.0f, 1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,  
};

static unsigned int cube_idx[24] = {
    // Front face edges
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    // Back face edges
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    // Connecting edges
    0, 4,
    1, 5,
    2, 6,
    3, 7
};

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    srand(time(NULL)); 

    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    int w_width = 1280, w_height = 720;

    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("eray", w_width, w_height, 0);
    renderer = SDL_CreateRenderer(win, NULL);

    // Showed picked renderer.
    const char* rname = SDL_GetRendererName(renderer);
    fprintf(stdout, "[SDL RENDERER]: %s\n", rname);
    fflush(stdout);

    Eray_Canvas ec = eray_init(pixels, CANVAS_WIDTH, CANVAS_HEIGHT, CANVAS_WIDTH*sizeof(uint32_t));
    float aspect_ratio = eray_aspect_ratio(ec);
    printf("[ASPECT RATIO: %f]\n", aspect_ratio);
   
    // Convert Canvas to SDL texture.
    SDL_Texture *text = NULL;
    SDL_Surface *surf = NULL;
    surf = SDL_CreateSurfaceFrom(ec.width, ec.height, SDL_PIXELFORMAT_RGBA32, pixels, ec.stride);
    text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    
    int done = 0;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_Q)) {
                done = 1;
                break;
            }

        }

        float model[24] = {0};
        memcpy(model, cube, sizeof(cube));

        // ROTATE CUBE.
        static float rot = 0.0;
        rot += 0.01f;

        for (size_t i = 0; i < ARRLEN(model); i+=3) {
            float x = model[i];
            float z = model[i+2];
            model[i]   = cos(rot) * x - sin(rot) * z;
            model[i+2] = sin(rot) * x + cos(rot) * z;
        }

        // TRANSLATE CUBE.
        for (size_t i = 0; i < ARRLEN(model); i+=3) {
            model[i+2] += -4.0f;
        }

        int points_loop[16] = {0};
        int p = 0;
        for (size_t vertex = 0; vertex < ARRLEN(model); vertex+=3) {
            if (model[vertex+2] == 0.0f) continue; // clipping XD
            float x_proj = (model[vertex] / -model[vertex+2]) / aspect_ratio;
            float y_proj = model[vertex+1] / -model[vertex+2];
            float x_proj_remap = (x_proj + 1) / 2;
            float y_proj_remap = (y_proj + 1) / 2;
            int x_proj_pix = (int)(x_proj_remap * ec.width);
            int y_proj_pix = (int)(y_proj_remap * ec.height);
            points_loop[p] = x_proj_pix;
            points_loop[p+1] = y_proj_pix;
            p+=2;
        }
        
        eray_fill(ec, ERAY_COLOR_DARKGRAY);
        for (size_t i = 0; i < ARRLEN(cube_idx);) {
            int idx1 = cube_idx[i] * 2;
            int idx2 = cube_idx[i+1] * 2;
            int x1 = points_loop[idx1];
            int y1 = points_loop[idx1+1];
            int x2 = points_loop[idx2];
            int y2 = points_loop[idx2+1];
            eray_plot_line(ec, x1, y1, x2, y2, ERAY_COLOR_BLUE);
            i+=2;
        }   

        SDL_Texture *text = NULL;
        SDL_Surface *surf = NULL;
        surf = SDL_CreateSurfaceFrom(ec.width, ec.height, SDL_PIXELFORMAT_RGBA32, pixels, ec.stride);
        text = SDL_CreateTextureFromSurface(renderer, surf);


        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, text, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_DestroySurface(surf);
        SDL_DestroyTexture(text);

        usleep(10000);
    }

    SDL_DestroyTexture(text);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}
