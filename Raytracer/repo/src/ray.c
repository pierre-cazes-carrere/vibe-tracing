#include "ray.h"

Ray ray_create(Vec3 origin, Vec3 direction) {
    return (Ray){origin, vec3_normalize(direction)};
}

Vec3 ray_at(Ray ray, float t) {
    return vec3_add(ray.origin, vec3_mul(ray.direction, t));
}
