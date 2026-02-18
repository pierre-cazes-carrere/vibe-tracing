#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_utils.h"
#include "image.h"
#include "ray.h"
#include "sphere.h"

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600

int main() {
    printf("Ray Tracer - Version C\n");
    printf("Image: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // Create image
    Image* img = image_create(IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // Create scene with spheres
    SphereList* scene = sphere_list_create(10);
    sphere_list_add(scene, sphere_create(vec3_new(0.0f, 0.0f, -5.0f), 1.0f, 0));
    sphere_list_add(scene, sphere_create(vec3_new(-2.0f, 0.0f, -5.0f), 1.0f, 1));
    sphere_list_add(scene, sphere_create(vec3_new(2.0f, 0.0f, -5.0f), 1.0f, 2));
    sphere_list_add(scene, sphere_create(vec3_new(0.0f, -100.0f, 0.0f), 99.0f, 3));
    
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
        if (y % 60 == 0) {
            printf("  Row %d/%d\n", y, IMAGE_HEIGHT);
        }
        
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            float u = (float)x / IMAGE_WIDTH;
            float v = (float)y / IMAGE_HEIGHT;
            
            Vec3 ray_dir = vec3_add(lower_left, vec3_mul(viewport_width_vec, u));
            ray_dir = vec3_add(ray_dir, vec3_mul(viewport_height_vec, v));
            
            Ray ray = ray_create(camera_pos, ray_dir);
            RayHit hit = {0};
            
            if (sphere_list_hit_any(scene, ray, 0.0f, 1000.0f, &hit)) {
                // Basic shading: use normal as color
                float r = (hit.normal.x + 1.0f) / 2.0f;
                float g = (hit.normal.y + 1.0f) / 2.0f;
                float b = (hit.normal.z + 1.0f) / 2.0f;
                image_set_pixel(img, x, y, (Color){r, g, b});
            } else {
                // Sky gradient
                float t = (ray.direction.y + 1.0f) / 2.0f;
                float r = 1.0f * (1.0f - t) + 0.5f * t;
                float g = 1.0f * (1.0f - t) + 0.7f * t;
                float b_val = 1.0f * (1.0f - t) + 1.0f * t;
                image_set_pixel(img, x, y, (Color){r, g, b_val});
            }
        }
    }
    
    // Save to PPM
    if (image_write_ppm(img, "output.ppm")) {
        printf("Image saved to output.ppm\n");
    } else {
        printf("Error saving image\n");
    }
    
    // Cleanup
    sphere_list_free(scene);
    image_free(img);
    
    return 0;
}
