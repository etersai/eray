#ifndef DATA_FONT_H_
#define DATA_FONT_H_

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

#endif /* DATA_FONT_H_ */
