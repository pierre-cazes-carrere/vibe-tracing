#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "math_utils.h"
#include "ray.h"
#include "sphere.h"
#include "material.h"

#define MAX_MATERIALS 64
#define MAX_DEPTH 5

typedef struct {
    Material materials[MAX_MATERIALS];
    int material_count;
    SphereList* scene;
} Scene;

Scene* scene_create();
void scene_free(Scene* scene);
int scene_add_material(Scene* scene, Material mat);
void scene_add_object(Scene* scene, Sphere sphere);

Color trace_ray(Ray ray, Scene* scene, int depth);

#endif
