#ifndef DISPLAY_H
#define DISPLAY_H

#include "image.h"

typedef struct {
    int width;
    int height;
    int is_open;
} Display;

Display* display_create(int width, int height, const char* title);
void display_free(Display* disp);
void display_update(Display* disp, Image* img);
void display_clear(Display* disp);
int display_is_open(Display* disp);

#endif
