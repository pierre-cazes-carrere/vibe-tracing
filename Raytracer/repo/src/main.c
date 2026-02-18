#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "math_utils.h"
#include "image.h"
#include "ray.h"
#include "sphere.h"
#include "material.h"
#include "raytracer.h"
#include "display.h"
#include "game.h"

#define IMAGE_WIDTH 400
#define IMAGE_HEIGHT 300
#define SAMPLES_PER_PIXEL 8

int main() {
    srand(time(NULL));
    
    printf("Ray Tracer Game - Version C\n");
    printf("Image: %dx%d, Samples: %d\n", IMAGE_WIDTH, IMAGE_HEIGHT, SAMPLES_PER_PIXEL);
    
    // Create game state
    GameState* game = game_create();
    
    // Create scene
    Scene* scene = scene_create();
    
    // Add materials
    int mat_player = scene_add_material(scene, material_diffuse(vec3_new(0.2f, 0.8f, 0.2f)));
    int mat_enemy = scene_add_material(scene, material_diffuse(vec3_new(0.8f, 0.2f, 0.2f)));
    int mat_gold = scene_add_material(scene, material_metal(vec3_new(1.0f, 0.8f, 0.0f), 0.15f));
    int mat_ground = scene_add_material(scene, material_diffuse(vec3_new(0.6f, 0.6f, 0.6f)));
    
    game->player_material_id = mat_player;
    
    // Populate scene with game objects
    game_populate_scene(game, scene);
    
    // Create image
    Image* img = image_create(IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // Create display
    Display* display = display_create(IMAGE_WIDTH, IMAGE_HEIGHT, "Ray Tracer Game");
    
    // Camera setup
    Vec3 camera_pos = vec3_new(0.0f, 1.5f, 3.0f);
    float aspect = (float)IMAGE_WIDTH / IMAGE_HEIGHT;
    float fov = 60.0f;
    float tan_half_fov = tanf((fov / 2.0f) * 3.14159f / 180.0f);
    
    Vec3 viewport_height_vec = vec3_mul(vec3_new(0.0f, 1.0f, 0.0f), 2.0f * tan_half_fov);
    Vec3 viewport_width_vec = vec3_mul(vec3_new(1.0f, 0.0f, 0.0f), aspect * 2.0f * tan_half_fov);
    
    Vec3 lower_left = vec3_sub(camera_pos, vec3_mul(viewport_width_vec, 0.5f));
    lower_left = vec3_sub(lower_left, vec3_mul(viewport_height_vec, 0.5f));
    lower_left = vec3_sub(lower_left, vec3_new(0.0f, 0.0f, 1.0f));
    
    // Update game
    game_update(game, 0.0f);
    game_populate_scene(game, scene);
    
    // Render
    printf("Rendering game scene...\n");
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        if (y % 30 == 0) {
            printf("  Row %d/%d\n", y, IMAGE_HEIGHT);
        }
        
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            Color pixel_color = {0.0f, 0.0f, 0.0f};
            
            // Antialiasing with multiple samples
            for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
                float u = (x + random_float()) / IMAGE_WIDTH;
                float v = (y + random_float()) / IMAGE_HEIGHT;
                
                Vec3 ray_dir = vec3_add(lower_left, vec3_mul(viewport_width_vec, u));
                ray_dir = vec3_add(ray_dir, vec3_mul(viewport_height_vec, v));
                
                Ray ray = ray_create(camera_pos, ray_dir);
                Color col = trace_ray(ray, scene, MAX_DEPTH);
                
                pixel_color.r += col.r;
                pixel_color.g += col.g;
                pixel_color.b += col.b;
            }
            
            // Average samples and apply gamma correction
            pixel_color.r = sqrtf(pixel_color.r / SAMPLES_PER_PIXEL);
            pixel_color.g = sqrtf(pixel_color.g / SAMPLES_PER_PIXEL);
            pixel_color.b = sqrtf(pixel_color.b / SAMPLES_PER_PIXEL);
            
            image_set_pixel(img, x, y, pixel_color);
        }
    }
    
    // Display stats
    printf("Score: %d\n", game->score);
    
    // Save to BMP
    display_update(display, img);
    printf("Image saved to output.bmp\n");
    
    // Cleanup
    display_free(display);
    game_free(game);
    scene_free(scene);
    image_free(img);
    
    return 0;
}
