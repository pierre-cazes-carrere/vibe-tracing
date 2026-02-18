#include "renderer.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

Renderer* renderer_create(int width, int height) {
    Renderer* renderer = (Renderer*)malloc(sizeof(Renderer));
    renderer->width = width;
    renderer->height = height;
    renderer->framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    return renderer;
}

void renderer_free(Renderer* renderer) {
    if (renderer) {
        free(renderer->framebuffer);
        free(renderer);
    }
}

void renderer_clear(Renderer* renderer, uint32_t color) {
    if (!renderer) return;
    for (int i = 0; i < renderer->width * renderer->height; i++) {
        renderer->framebuffer[i] = color;
    }
}

static uint32_t color_from_rgb(float r, float g, float b) {
    int ri = (int)(r * 255.0f);
    int gi = (int)(g * 255.0f);
    int bi = (int)(b * 255.0f);
    
    ri = ri > 255 ? 255 : (ri < 0 ? 0 : ri);
    gi = gi > 255 ? 255 : (gi < 0 ? 0 : gi);
    bi = bi > 255 ? 255 : (bi < 0 ? 0 : bi);
    
    return (ri << 16) | (gi << 8) | bi;
}

static void set_pixel(Renderer* renderer, int x, int y, uint32_t color) {
    if (x >= 0 && x < renderer->width && y >= 0 && y < renderer->height) {
        renderer->framebuffer[y * renderer->width + x] = color;
    }
}

void renderer_draw_sphere(Renderer* renderer, Vec3 pos, float radius, uint32_t color, Vec3 light) {
    if (!renderer) return;
    
    // Simple sphere projection (orthogonal)
    int cx = (int)(pos.x * 50 + renderer->width / 2);
    int cy = (int)(-pos.z * 50 + renderer->height / 2);
    int r = (int)(radius * 50);
    
    // Draw filled circle
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                int px = cx + x;
                int py = cy + y;
                
                if (px >= 0 && px < renderer->width && py >= 0 && py < renderer->height) {
                    // Simple lighting
                    float depth = sqrtf((float)(x * x + y * y)) / r;
                    float shade = 1.0f - depth * 0.3f;
                    
                    uint32_t base_color = color;
                    uint32_t r_val = ((base_color >> 16) & 0xFF);
                    uint32_t g_val = ((base_color >> 8) & 0xFF);
                    uint32_t b_val = (base_color & 0xFF);
                    
                    r_val = (uint32_t)(r_val * shade);
                    g_val = (uint32_t)(g_val * shade);
                    b_val = (uint32_t)(b_val * shade);
                    
                    uint32_t shaded_color = (r_val << 16) | (g_val << 8) | b_val;
                    set_pixel(renderer, px, py, shaded_color);
                }
            }
        }
    }
}

void renderer_draw_game(Renderer* renderer, GameState* game, Vec3 camera_pos, Vec3 camera_dir) {
    if (!renderer || !game) return;
    
    // Sky gradient background (top)
    for (int y = 0; y < renderer->height / 2; y++) {
        float t = (float)y / (renderer->height / 2);
        uint32_t sky_color = color_from_rgb(
            0.2f + 0.8f * (1.0f - t),
            0.5f + 0.5f * (1.0f - t),
            1.0f
        );
        
        for (int x = 0; x < renderer->width; x++) {
            set_pixel(renderer, x, y, sky_color);
        }
    }
    
    // Ground (bottom)
    uint32_t ground_color = color_from_rgb(0.2f, 0.6f, 0.2f);
    for (int y = renderer->height / 2; y < renderer->height; y++) {
        for (int x = 0; x < renderer->width; x++) {
            set_pixel(renderer, x, y, ground_color);
        }
    }
    
    Vec3 light = vec3_new(1.0f, 1.0f, 1.0f);
    
    // Draw enemies
    uint32_t enemy_color = color_from_rgb(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < game->enemy_count; i++) {
        Vec3 enemy_pos = game->enemies[i].position;
        enemy_pos.y += game->enemies[i].bob_offset;
        renderer_draw_sphere(renderer, enemy_pos, game->enemies[i].radius, enemy_color, light);
    }
    
    // Draw collectibles  
    uint32_t collectible_color = color_from_rgb(1.0f, 1.0f, 0.0f);
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].is_collected) continue;
        renderer_draw_sphere(renderer, game->collectibles[i].position, 
                           game->collectibles[i].radius, collectible_color, light);
    }
    
    // Draw player (larger, more visible)
    uint32_t player_color = color_from_rgb(0.0f, 1.0f, 0.0f);
    renderer_draw_sphere(renderer, game->player_pos, game->player_radius, player_color, light);
}
