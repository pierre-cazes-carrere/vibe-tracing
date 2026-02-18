#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    uint32_t* framebuffer;
    int is_open;
    int keys[256];  // Key state array
} GameWindow;

GameWindow* window_create(int width, int height, const char* title);
void window_free(GameWindow* window);
void window_update(GameWindow* window);
void window_draw_frame(GameWindow* window, uint32_t* pixels);
int window_is_open(GameWindow* window);
int window_key_pressed(GameWindow* window, int key);
void window_clear(GameWindow* window, uint32_t color);

#endif
