#include "raytracer.h"
#include <stdlib.h>

static Vec3 random_in_unit_sphere() {
    while (1) {
        Vec3 p = vec3_new(
            random_float_range(-1.0f, 1.0f),
            random_float_range(-1.0f, 1.0f),
            random_float_range(-1.0f, 1.0f)
        );
        if (vec3_length(p) < 1.0f) {
            return p;
        }
    }
}

static Vec3 random_unit_vector() {
    return vec3_normalize(random_in_unit_sphere());
}

Scene* scene_create() {
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    scene->material_count = 0;
    scene->scene = sphere_list_create(32);
    return scene;
}

void scene_free(Scene* scene) {
    if (scene) {
        sphere_list_free(scene->scene);
        free(scene);
    }
}

int scene_add_material(Scene* scene, Material mat) {
    if (scene->material_count >= MAX_MATERIALS) {
        return -1;
    }
    int id = scene->material_count++;
    scene->materials[id] = mat;
    return id;
}

void scene_add_object(Scene* scene, Sphere sphere) {
    sphere_list_add(scene->scene, sphere);
}

Color trace_ray(Ray ray, Scene* scene, int depth) {
    if (depth <= 0) {
        return (Color){0.0f, 0.0f, 0.0f};
    }
    
    RayHit hit = {0};
    if (sphere_list_hit_any(scene->scene, ray, 0.001f, 1e6f, &hit)) {
        Material mat = scene->materials[hit.material_id];
        
        if (mat.type == MAT_DIFFUSE) {
            // Diffuse scattering
            Vec3 scatter_dir = vec3_add(hit.normal, random_unit_vector());
            if (vec3_length(scatter_dir) < 0.001f) {
                scatter_dir = hit.normal;
            }
            Ray scattered = ray_create(hit.point, scatter_dir);
            Color recursive = trace_ray(scattered, scene, depth - 1);
            
            return (Color){
                mat.albedo.x * recursive.r * 0.7f,
                mat.albedo.y * recursive.g * 0.7f,
                mat.albedo.z * recursive.b * 0.7f
            };
        }
        else if (mat.type == MAT_METAL) {
            // Metal reflection
            Vec3 reflected = vec3_sub(ray.direction, 
                vec3_mul(hit.normal, 2.0f * vec3_dot(ray.direction, hit.normal)));
            
            Vec3 fuzz = vec3_mul(random_in_unit_sphere(), mat.roughness);
            reflected = vec3_add(reflected, fuzz);
            reflected = vec3_normalize(reflected);
            
            if (vec3_dot(reflected, hit.normal) > 0) {
                Ray scattered = ray_create(hit.point, reflected);
                Color recursive = trace_ray(scattered, scene, depth - 1);
                
                return (Color){
                    mat.albedo.x * recursive.r,
                    mat.albedo.y * recursive.g,
                    mat.albedo.z * recursive.b
                };
            } else {
                return (Color){0.0f, 0.0f, 0.0f};
            }
        }
        
        // Fallback shading
        float r = (hit.normal.x + 1.0f) / 2.0f;
        float g = (hit.normal.y + 1.0f) / 2.0f;
        float b = (hit.normal.z + 1.0f) / 2.0f;
        return (Color){r, g, b};
    }
    
    // Sky gradient
    float t = (ray.direction.y + 1.0f) / 2.0f;
    float r = 1.0f * (1.0f - t) + 0.5f * t;
    float g = 1.0f * (1.0f - t) + 0.7f * t;
    float b = 1.0f * (1.0f - t) + 1.0f * t;
    return (Color){r, g, b};
}
