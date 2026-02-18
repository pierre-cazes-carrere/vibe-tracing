#include "image.h"
#include <stdlib.h>
#include <stdio.h>

Image* image_create(int width, int height) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pixels = (Color*)malloc(width * height * sizeof(Color));
    return img;
}

void image_free(Image* img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

void image_set_pixel(Image* img, int x, int y, Color color) {
    if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
        img->pixels[y * img->width + x] = color;
    }
}

Color image_get_pixel(Image* img, int x, int y) {
    if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
        return img->pixels[y * img->width + x];
    }
    return (Color){0, 0, 0};
}

void image_clear(Image* img, Color color) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            image_set_pixel(img, x, y, color);
        }
    }
}

int image_write_ppm(Image* img, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }
    
    // PPM header
    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n", img->width, img->height);
    fprintf(file, "255\n");
    
    // Pixel data
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            Color c = image_get_pixel(img, x, y);
            int r = (int)(c.r * 255.0f);
            int g = (int)(c.g * 255.0f);
            int b = (int)(c.b * 255.0f);
            
            r = r > 255 ? 255 : (r < 0 ? 0 : r);
            g = g > 255 ? 255 : (g < 0 ? 0 : g);
            b = b > 255 ? 255 : (b < 0 ? 0 : b);
            
            fprintf(file, "%d %d %d ", r, g, b);
            if ((x + 1) % 5 == 0) {
                fprintf(file, "\n");
            }
        }
    }
    
    fclose(file);
    return 1;
}
