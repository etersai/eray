#ifndef ERAY_FONT_H_
#define ERAY_FONT_H_

#include "r_opengl.h"

// Generated using => https://evanw.github.io/font-texture-generator/
typedef struct internal_font_char {
    int codePoint;
    int x; 
    int y;
    int width;
    int height;
    int originX;
    int originY;
} internal_font_char;

typedef struct internal_font_data {
    const char *name;
    int size;
    int bold;
    int italic;
    int width;
    int height;
    int characterCount;
    internal_font_char *characters;
} internal_font_data;

extern const internal_font_data font_arial_white;

typedef struct {
    internal_font_data metadata;
    Texture texture;
} Fontek;

int font_create(Fontek* font, internal_font_data font_metadata, Texture texture);

#endif /* ERAY_FONT_H_ */
