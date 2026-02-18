#include "renderer.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

Renderer* renderer_create(int width, int height) {
    Renderer* renderer = (Renderer*)malloc(sizeof(Renderer));
    renderer->width = width;
    renderer->height = height;
    renderer->framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    renderer->depthbuffer = (float*)malloc(width * height * sizeof(float));
    return renderer;
}

void renderer_free(Renderer* renderer) {
    if (renderer) {
        free(renderer->framebuffer);
        free(renderer->depthbuffer);
        free(renderer);
    }
}

void renderer_clear(Renderer* renderer, uint32_t color) {
    if (!renderer) return;
    for (int i = 0; i < renderer->width * renderer->height; i++) {
        renderer->framebuffer[i] = color;
        renderer->depthbuffer[i] = 999999.0f;  // Far depth
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

static void set_pixel_depth(Renderer* renderer, int x, int y, uint32_t color, float depth) {
    if (x >= 0 && x < renderer->width && y >= 0 && y < renderer->height) {
        int idx = y * renderer->width + x;
        if (depth < renderer->depthbuffer[idx]) {
            renderer->framebuffer[idx] = color;
            renderer->depthbuffer[idx] = depth;
        }
    }
}

void renderer_draw_sky_gothic(Renderer* renderer, float time_of_day) {
    if (!renderer) return;
    
    // Gothic/dark atmosphere with time-based lighting
    float hour_cycle = fmodf(time_of_day / 24.0f, 1.0f);
    float darkness = sinf(hour_cycle * 3.14159f);  // 0 at night, 1 at day
    darkness = darkness < 0.0f ? 0.0f : darkness;
    
    for (int y = 0; y < renderer->height / 2; y++) {
        float t = (float)y / (renderer->height / 2);
        
        // Gothic dark purple/blue gradient
        float r_val = 0.1f + (0.2f * darkness) + (t * 0.15f);
        float g_val = 0.05f + (0.1f * darkness) + (t * 0.1f);
        float b_val = 0.3f + (0.2f * darkness) + (t * 0.3f);
        
        // Add some mist/fog effect
        float mist = sinf(y * 0.01f) * 0.05f;
        b_val += mist;
        
        uint32_t sky_color = color_from_rgb(r_val, g_val, b_val);
        
        for (int x = 0; x < renderer->width; x++) {
            set_pixel(renderer, x, y, sky_color);
        }
    }
}

void renderer_draw_ground_gothic(Renderer* renderer) {
    if (!renderer) return;
    
    // Dark gothic stone ground
    uint32_t ground_colors[3] = {
        color_from_rgb(0.15f, 0.15f, 0.2f),   // Dark stone
        color_from_rgb(0.2f, 0.2f, 0.25f),    // Lighter stone
        color_from_rgb(0.1f, 0.1f, 0.15f)     // Shadow
    };
    
    // Checkered stone pattern
    for (int y = renderer->height / 2; y < renderer->height; y++) {
        for (int x = 0; x < renderer->width; x++) {
            int pattern = ((x / 32) + (y / 32)) % 3;
            set_pixel(renderer, x, y, ground_colors[pattern]);
        }
    }
}

static void renderer_draw_box_3d(Renderer* renderer, Vec3 pos, Vec3 size, uint32_t color) {
    if (!renderer) return;
    
    // Project center to 2D
    int cx = (int)(pos.x * 50 + renderer->width / 2);
    int cy = (int)(-pos.z * 50 + renderer->height / 2);
    
    // Dimensions in screen space
    int half_w = (int)(size.x * 50 / 2);
    int half_h = (int)(size.z * 50 / 2);
    int height_pixels = (int)(size.y * 50);
    
    // Draw front face (solid)
    for (int y = -height_pixels; y <= 0; y++) {
        for (int x = -half_w; x <= half_w; x++) {
            float depth_factor = 1.0f - (fabsf((float)x) / half_w) * 0.2f;
            
            uint32_t r_val = ((color >> 16) & 0xFF);
            uint32_t g_val = ((color >> 8) & 0xFF);
            uint32_t b_val = (color & 0xFF);
            
            r_val = (uint32_t)(r_val * depth_factor);
            g_val = (uint32_t)(g_val * depth_factor);
            b_val = (uint32_t)(b_val * depth_factor);
            
            uint32_t shaded = (r_val << 16) | (g_val << 8) | b_val;
            set_pixel_depth(renderer, cx + x, cy + y, shaded, pos.z);
        }
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
                    set_pixel_depth(renderer, px, py, shaded_color, pos.z);
                }
            }
        }
    }
}

static void renderer_draw_cylinder(Renderer* renderer, Vec3 start, Vec3 end, float radius, uint32_t color) {
    if (!renderer) return;
    
    // Simple cylinder rendering as a thick line
    int x1 = (int)(start.x * 50 + renderer->width / 2);
    int y1 = (int)(-start.z * 50 + renderer->height / 2);
    int x2 = (int)(end.x * 50 + renderer->width / 2);
    int y2 = (int)(-end.z * 50 + renderer->height / 2);
    
    int r = (int)(radius * 50);
    
    // Bresenham line algorithm with thickness
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    float avg_z = (start.z + end.z) * 0.5f;
    
    while (1) {
        // Draw circle at this point
        for (int cy = -r; cy <= r; cy++) {
            for (int cx = -r; cx <= r; cx++) {
                if (cx * cx + cy * cy <= r * r) {
                    int px = x1 + cx;
                    int py = y1 + cy;
                    float shade = 1.0f - (fabsf((float)cx) / r) * 0.2f;
                    
                    uint32_t r_val = ((color >> 16) & 0xFF);
                    uint32_t g_val = ((color >> 8) & 0xFF);
                    uint32_t b_val = (color & 0xFF);
                    
                    r_val = (uint32_t)(r_val * shade);
                    g_val = (uint32_t)(g_val * shade);
                    b_val = (uint32_t)(b_val * shade);
                    
                    uint32_t shaded = (r_val << 16) | (g_val << 8) | b_val;
                    set_pixel_depth(renderer, px, py, shaded, avg_z);
                }
            }
        }
        
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void renderer_draw_humanoid_3d(Renderer* renderer, Humanoid* humanoid, uint32_t color) {
    if (!renderer || !humanoid) return;
    
    // Draw legs (back to front for proper depth)
    float limb_radius = humanoid->limb_scale * 0.6f;
    
    // Left leg
    renderer_draw_cylinder(renderer, humanoid->torso_pos, humanoid->left_leg_pos, limb_radius, color);
    
    // Right leg  
    renderer_draw_cylinder(renderer, humanoid->torso_pos, humanoid->right_leg_pos, limb_radius, color);
    
    // Draw torso as a cylinder
    Vec3 torso_top = vec3_add(humanoid->torso_pos, vec3_new(0.0f, 0.3f, 0.0f));
    renderer_draw_cylinder(renderer, humanoid->torso_pos, torso_top, humanoid->limb_scale * 1.2f, color);
    
    // Draw arms
    renderer_draw_cylinder(renderer, torso_top, humanoid->left_arm_pos, limb_radius, color);
    renderer_draw_cylinder(renderer, torso_top, humanoid->right_arm_pos, limb_radius, color);
    
    // Draw head as sphere
    uint32_t head_color = color;  // Skin tone
    renderer_draw_sphere(renderer, humanoid->head_pos, humanoid->head_radius, head_color, vec3_new(1.0f, 1.0f, 1.0f));
}

void renderer_draw_structure_3d(Renderer* renderer, Structure* structure) {
    if (!renderer || !structure) return;
    
    // Color based on structure type for gothic aesthetics
    uint32_t color;
    switch (structure->type) {
        case ARCH_COLUMN:
            color = color_from_rgb(0.3f, 0.3f, 0.35f);  // Gray stone
            break;
        case ARCH_WALL:
            color = color_from_rgb(0.25f, 0.25f, 0.3f);  // Dark stone
            break;
        case ARCH_ARCH:
            color = color_from_rgb(0.35f, 0.35f, 0.4f);  // Lighter arch
            break;
        case ARCH_TOWER:
            color = color_from_rgb(0.2f, 0.2f, 0.25f);  // Very dark tower
            break;
        case ARCH_RUIN:
            color = color_from_rgb(0.28f, 0.28f, 0.32f);  // Ruin stone
            break;
        default:
            color = color_from_rgb(0.3f, 0.3f, 0.3f);
    }
    
    // Draw as 3D box
    renderer_draw_box_3d(renderer, structure->position, structure->size, color);
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
                set_pixel_depth(renderer, px + x, py + y, color, pos.z);
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
            set_pixel_depth(renderer, cx + x, cy + y, color, pos.z);
        }
    }
}

void renderer_draw_game(Renderer* renderer, GameState* game, Environment* env, Vec3 camera_pos, Vec3 camera_dir) {
    if (!renderer || !game || !env) return;
    
    // Draw gothic sky
    renderer_draw_sky_gothic(renderer, env->time_of_day);
    
    // Draw gothic ground
    renderer_draw_ground_gothic(renderer);
    
    // Draw all architectural structures
    for (int i = 0; i < env->structure_count; i++) {
        renderer_draw_structure_3d(renderer, &env->structures[i]);
    }
    
    // Draw enemies as humanoids
    uint32_t enemy_color = color_from_rgb(0.8f, 0.1f, 0.1f);
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].radius <= 0.0f) continue;  // Skip dead enemies
        
        Vec3 enemy_pos = game->enemies[i].position;
        Vec3 to_player = vec3_sub(game->player.position, enemy_pos);
        Vec3 enemy_dir = vec3_normalize(to_player);
        
        // Create temporary humanoid for rendering
        Humanoid enemy_h = humanoid_create(enemy_pos, enemy_dir);
        enemy_h.animation_time = env->time_of_day * 10.0f + i;  // Offset for varied animation
        humanoid_compute_parts(&enemy_h);
        
        renderer_draw_humanoid_3d(renderer, &enemy_h, enemy_color);
    }
    
    // Draw projectiles
    uint32_t projectile_color = color_from_rgb(1.0f, 0.8f, 0.0f);
    for (int i = 0; i < game->projectile_count; i++) {
        renderer_draw_projectile(renderer, game->projectiles[i].position, projectile_color);
    }
    
    // Draw player as humanoid
    uint32_t player_color = color_from_rgb(0.0f, 0.8f, 0.0f);
    Humanoid player_h = humanoid_create(game->player.position, game->player.direction);
    player_h.animation_time = env->time_of_day * 10.0f;
    humanoid_compute_parts(&player_h);
    renderer_draw_humanoid_3d(renderer, &player_h, player_color);
}
