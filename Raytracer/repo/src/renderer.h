#ifndef RENDERER_H
#define RENDERER_H

#include "math_utils.h"
#include "sphere.h"
#include "game.h"
#include "humanoid.h"
#include "environment.h"
#include <stdint.h>

typedef struct {
    int width;
    int height;
    uint32_t* framebuffer;
    float* depthbuffer;  // For depth sorting
} Renderer;

Renderer* renderer_create(int width, int height);
void renderer_free(Renderer* renderer);
void renderer_clear(Renderer* renderer, uint32_t color);
void renderer_draw_sky_gothic(Renderer* renderer, float time_of_day);
void renderer_draw_ground_gothic(Renderer* renderer);
void renderer_draw_structure_3d(Renderer* renderer, Structure* structure);
void renderer_draw_sphere(Renderer* renderer, Vec3 pos, float radius, uint32_t color, Vec3 light);
void renderer_draw_humanoid_3d(Renderer* renderer, Humanoid* humanoid, uint32_t color);
void renderer_draw_projectile(Renderer* renderer, Vec3 pos, uint32_t color);
void renderer_draw_game(Renderer* renderer, GameState* game, Environment* env, Vec3 camera_pos, Vec3 camera_dir);

#endif
