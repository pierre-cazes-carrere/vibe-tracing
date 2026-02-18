#ifndef RAY_H
#define RAY_H

#include "math_utils.h"

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    Vec3 point;
    Vec3 normal;
    float t;
    int hit;
} RayHit;

Ray ray_create(Vec3 origin, Vec3 direction);
Vec3 ray_at(Ray ray, float t);

#endif
