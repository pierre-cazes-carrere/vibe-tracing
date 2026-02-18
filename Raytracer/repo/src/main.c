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

#define IMAGE_WIDTH 400
#define IMAGE_HEIGHT 300
#define SAMPLES_PER_PIXEL 8

int main() {
    srand(time(NULL));
    
    printf("Ray Tracer - Version C\n");
    printf("Image: %dx%d, Samples: %d\n", IMAGE_WIDTH, IMAGE_HEIGHT, SAMPLES_PER_PIXEL);
    
    // Create scene
    Scene* scene = scene_create();
    
    // Add materials
    int mat_red = scene_add_material(scene, material_diffuse(vec3_new(0.8f, 0.1f, 0.1f)));
    int mat_blue = scene_add_material(scene, material_diffuse(vec3_new(0.1f, 0.2f, 0.8f)));
    int mat_green = scene_add_material(scene, material_diffuse(vec3_new(0.1f, 0.8f, 0.1f)));
    int mat_ground = scene_add_material(scene, material_diffuse(vec3_new(0.9f, 0.9f, 0.9f)));
    int mat_metal = scene_add_material(scene, material_metal(vec3_new(0.8f, 0.8f, 0.8f), 0.2f));
    
    // Add objects
    scene_add_object(scene, sphere_create(vec3_new(0.0f, -100.5f, -1.0f), 100.0f, mat_ground));
    scene_add_object(scene, sphere_create(vec3_new(0.0f, 0.0f, -1.0f), 0.5f, mat_blue));
    scene_add_object(scene, sphere_create(vec3_new(-1.0f, 0.0f, -1.0f), 0.5f, mat_red));
    scene_add_object(scene, sphere_create(vec3_new(1.0f, 0.0f, -1.0f), 0.5f, mat_green));
    scene_add_object(scene, sphere_create(vec3_new(0.5f, 0.3f, 0.5f), 0.3f, mat_metal));
    
    // Create image
    Image* img = image_create(IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // Camera setup
    Vec3 camera_pos = vec3_new(0.0f, 0.0f, 0.0f);
    float aspect = (float)IMAGE_WIDTH / IMAGE_HEIGHT;
    float fov = 90.0f;
    float tan_half_fov = tanf((fov / 2.0f) * 3.14159f / 180.0f);
    
    Vec3 viewport_height_vec = vec3_mul(vec3_new(0.0f, 1.0f, 0.0f), 2.0f * tan_half_fov);
    Vec3 viewport_width_vec = vec3_mul(vec3_new(1.0f, 0.0f, 0.0f), aspect * 2.0f * tan_half_fov);
    
    Vec3 lower_left = vec3_sub(camera_pos, vec3_mul(viewport_width_vec, 0.5f));
    lower_left = vec3_sub(lower_left, vec3_mul(viewport_height_vec, 0.5f));
    lower_left = vec3_sub(lower_left, vec3_new(0.0f, 0.0f, 1.0f));
    
    // Render
    printf("Rendering...\n");
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
    
    // Save to PPM
    if (image_write_ppm(img, "output.ppm")) {
        printf("Image saved to output.ppm\n");
    } else {
        printf("Error saving image\n");
    }
    
    // Cleanup
    scene_free(scene);
    image_free(img);
    
    return 0;
}
