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

void renderer_draw_humanoid(Renderer* renderer, Vec3 pos, Vec3 direction, uint32_t color) {
    if (!renderer) return;
    
    // Project position to 2D
    int cx = (int)(pos.x * 50 + renderer->width / 2);
    int cy = (int)(-pos.z * 50 + renderer->height / 2);
    
    // Draw humanoid as a simple figure: head + body + arms/legs
    int head_r = 8;
    
    // Draw head (circle)
    for (int y = -head_r; y <= head_r; y++) {
        for (int x = -head_r; x <= head_r; x++) {
            if (x * x + y * y <= head_r * head_r) {
                set_pixel(renderer, cx + x, cy + y - 15, color);
            }
        }
    }
    
    // Draw body (rectangle)
    for (int y = -12; y <= 12; y++) {
        for (int x = -6; x <= 6; x++) {
            set_pixel(renderer, cx + x, cy + y, color);
        }
    }
    
    // Draw direction indicator (eye/weapon pointing)
    int dir_scale = 15;
    int aim_x = (int)(direction.x * dir_scale);
    int aim_z = (int)(direction.z * dir_scale);
    int aim_y = (int)(-aim_z * 50);
    aim_z = aim_x;
    aim_x = aim_z;
    
    for (int i = 0; i < 8; i++) {
        set_pixel(renderer, cx + i * (aim_x > 0 ? 1 : -1), cy + i * (aim_y > 0 ? 1 : -1), 
                 color_from_rgb(1.0f, 1.0f, 0.0f));
    }
}

void renderer_draw_projectile(Renderer* renderer, Vec3 pos, uint32_t color) {
    if (!renderer) return;
    
    // Project to 2D
    int px = (int)(pos.x * 50 + renderer->width / 2);
    int py = (int)(-pos.z * 50 + renderer->height / 2);
    
    // Draw small projectile (2x2 or 3x3)
    for (int y = -2; y <= 2; y++) {
        for (int x = -2; x <= 2; x++) {
            if (x * x + y * y <= 4) {
                set_pixel(renderer, px + x, py + y, color);
            }
        }
    }
}

void renderer_draw_obstacle(Renderer* renderer, Vec3 pos, Vec3 size, uint32_t color) {
    if (!renderer) return;
    
    // Project to 2D
    int cx = (int)(pos.x * 50 + renderer->width / 2);
    int cy = (int)(-pos.z * 50 + renderer->height / 2);
    
    // Draw as rectangle
    int half_w = (int)(size.x * 50 / 2);
    int half_h = (int)(size.z * 50 / 2);
    
    for (int y = -half_h; y <= half_h; y++) {
        for (int x = -half_w; x <= half_w; x++) {
            set_pixel(renderer, cx + x, cy + y, color);
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
    
    // Draw obstacles (walls/environment)
    uint32_t wall_color = color_from_rgb(0.5f, 0.5f, 0.5f);
    for (int i = 0; i < game->obstacle_count; i++) {
        renderer_draw_obstacle(renderer, game->obstacles[i].position, game->obstacles[i].size, wall_color);
    }
    
    // Draw enemies as humanoids
    uint32_t enemy_color = color_from_rgb(1.0f, 0.2f, 0.2f);
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].radius <= 0.0f) continue;  // Skip dead enemies
        
        Vec3 enemy_pos = game->enemies[i].position;
        enemy_pos.y += game->enemies[i].bob_offset;
        
        // Enemy direction (towards player)
        Vec3 to_player = vec3_sub(game->player.position, enemy_pos);
        Vec3 enemy_dir = vec3_normalize(to_player);
        
        renderer_draw_humanoid(renderer, enemy_pos, enemy_dir, enemy_color);
    }
    
    // Draw projectiles
    uint32_t projectile_color = color_from_rgb(1.0f, 1.0f, 0.0f);
    for (int i = 0; i < game->projectile_count; i++) {
        renderer_draw_projectile(renderer, game->projectiles[i].position, projectile_color);
    }
    
    // Draw player as humanoid
    uint32_t player_color = color_from_rgb(0.0f, 1.0f, 0.0f);
    renderer_draw_humanoid(renderer, game->player.position, game->player.direction, player_color);
}
