#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct {
    float r, g, b;
} Color;

typedef struct {
    int width;
    int height;
    Color* pixels;
} Image;

Image* image_create(int width, int height);
void image_free(Image* img);
void image_set_pixel(Image* img, int x, int y, Color color);
Color image_get_pixel(Image* img, int x, int y);
void image_clear(Image* img, Color color);
int image_write_ppm(Image* img, const char* filename);

#endif
