#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float r, g, b;
} Color;

// Vector operations
Vec3 vec3_new(float x, float y, float z);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 v, float s);
float vec3_dot(Vec3 a, Vec3 b);
float vec3_length(Vec3 v);
Vec3 vec3_normalize(Vec3 v);
Vec3 vec3_cross(Vec3 a, Vec3 b);

#endif
