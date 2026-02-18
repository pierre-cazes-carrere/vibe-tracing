#include "sphere.h"
#include <stdlib.h>
#include <math.h>

Sphere sphere_create(Vec3 center, float radius, int material_id) {
    return (Sphere){center, radius, material_id};
}

int sphere_hit(Sphere sphere, Ray ray, float t_min, float t_max, RayHit* hit) {
    Vec3 oc = vec3_sub(ray.origin, sphere.center);
    
    float a = vec3_dot(ray.direction, ray.direction);
    float b = 2.0f * vec3_dot(oc, ray.direction);
    float c = vec3_dot(oc, oc) - sphere.radius * sphere.radius;
    
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return 0;
    }
    
    float sqrtd = sqrtf(discriminant);
    float root = (-b - sqrtd) / (2 * a);
    
    if (root < t_min || root > t_max) {
        root = (-b + sqrtd) / (2 * a);
        if (root < t_min || root > t_max) {
            return 0;
        }
    }
    
    hit->t = root;
    hit->point = ray_at(ray, root);
    Vec3 outward_normal = vec3_mul(vec3_sub(hit->point, sphere.center), 1.0f / sphere.radius);
    
    float dot = vec3_dot(ray.direction, outward_normal);
    if (dot > 0) {
        hit->normal = vec3_mul(outward_normal, -1.0f);
    } else {
        hit->normal = outward_normal;
    }
    
    hit->material_id = sphere.material_id;
    hit->hit = 1;
    return 1;
}

SphereList* sphere_list_create(int capacity) {
    SphereList* list = (SphereList*)malloc(sizeof(SphereList));
    list->spheres = (Sphere*)malloc(capacity * sizeof(Sphere));
    list->count = 0;
    list->capacity = capacity;
    return list;
}

void sphere_list_free(SphereList* list) {
    if (list) {
        free(list->spheres);
        free(list);
    }
}

void sphere_list_add(SphereList* list, Sphere sphere) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->spheres = (Sphere*)realloc(list->spheres, list->capacity * sizeof(Sphere));
    }
    list->spheres[list->count++] = sphere;
}

int sphere_list_hit_any(SphereList* list, Ray ray, float t_min, float t_max, RayHit* hit) {
    int was_hit = 0;
    float closest = t_max;
    
    for (int i = 0; i < list->count; i++) {
        RayHit temp_hit;
        if (sphere_hit(list->spheres[i], ray, t_min, closest, &temp_hit)) {
            was_hit = 1;
            closest = temp_hit.t;
            *hit = temp_hit;
        }
    }
    
    return was_hit;
}
