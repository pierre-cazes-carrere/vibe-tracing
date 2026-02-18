#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"

typedef struct {
    Vec3 center;
    float radius;
    int material_id;
} Sphere;

typedef struct {
    Sphere* spheres;
    int count;
    int capacity;
} SphereList;

Sphere sphere_create(Vec3 center, float radius, int material_id);
int sphere_hit(Sphere sphere, Ray ray, float t_min, float t_max, RayHit* hit);

SphereList* sphere_list_create(int capacity);
void sphere_list_free(SphereList* list);
void sphere_list_add(SphereList* list, Sphere sphere);
int sphere_list_hit_any(SphereList* list, Ray ray, float t_min, float t_max, RayHit* hit);

#endif
