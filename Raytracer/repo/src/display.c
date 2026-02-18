#include "display.h"
#include <stdlib.h>
#include <stdio.h>

// BMP file header structures
#pragma pack(push, 1)
typedef struct {
    unsigned short signature;
    unsigned int file_size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int data_offset;
} BMPFileHeader;

typedef struct {
    unsigned int header_size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int image_size;
    int x_pixels_per_meter;
    int y_pixels_per_meter;
    unsigned int num_colors;
    unsigned int important_colors;
} BMPInfoHeader;
#pragma pack(pop)

Display* display_create(int width, int height, const char* title) {
    Display* disp = (Display*)malloc(sizeof(Display));
    disp->width = width;
    disp->height = height;
    disp->is_open = 1;
    return disp;
}

void display_free(Display* disp) {
    if (disp) {
        free(disp);
    }
}

void display_update(Display* disp, Image* img) {
    if (!disp || !img) return;
    
    // Save as BMP file
    FILE* file = fopen("output.bmp", "wb");
    if (!file) return;
    
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    
    // File header
    file_header.signature = 0x4D42; // "BM"
    file_header.reserved1 = 0;
    file_header.reserved2 = 0;
    file_header.data_offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = file_header.data_offset + (img->width * img->height * 3);
    
    // Info header
    info_header.header_size = sizeof(BMPInfoHeader);
    info_header.width = img->width;
    info_header.height = -img->height; // Negative for top-down
    info_header.planes = 1;
    info_header.bits_per_pixel = 24;
    info_header.compression = 0;
    info_header.image_size = img->width * img->height * 3;
    info_header.x_pixels_per_meter = 2835;
    info_header.y_pixels_per_meter = 2835;
    info_header.num_colors = 0;
    info_header.important_colors = 0;
    
    // Write headers
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);
    
    // Write pixel data (BGR format for BMP)
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            Color c = image_get_pixel(img, x, y);
            unsigned char b = (unsigned char)(c.b * 255.0f);
            unsigned char g = (unsigned char)(c.g * 255.0f);
            unsigned char r = (unsigned char)(c.r * 255.0f);
            
            fputc(b, file);
            fputc(g, file);
            fputc(r, file);
        }
    }
    
    fclose(file);
}

void display_clear(Display* disp) {
    // No-op for file-based display
}

int display_is_open(Display* disp) {
    return disp ? disp->is_open : 0;
}
