#ifndef RENDERER_H
#define RENDERER_H

#include "math_utils.h"
#include "sphere.h"
#include "game.h"
#include <stdint.h>

typedef struct {
    int width;
    int height;
    uint32_t* framebuffer;
} Renderer;

Renderer* renderer_create(int width, int height);
void renderer_free(Renderer* renderer);
void renderer_clear(Renderer* renderer, uint32_t color);
void renderer_draw_sphere(Renderer* renderer, Vec3 pos, float radius, uint32_t color, Vec3 light);
void renderer_draw_game(Renderer* renderer, GameState* game, Vec3 camera_pos, Vec3 camera_dir);

#endif
