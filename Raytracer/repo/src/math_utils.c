#include "math_utils.h"
#include <stdlib.h>

Vec3 vec3_new(float x, float y, float z) {
    return (Vec3){x, y, z};
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3_new(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3_new(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 v, float s) {
    return vec3_new(v.x * s, v.y * s, v.z * s);
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_length(Vec3 v) {
    return sqrtf(vec3_dot(v, v));
}

Vec3 vec3_normalize(Vec3 v) {
    float len = vec3_length(v);
    if (len > 0.0f) {
        return vec3_mul(v, 1.0f / len);
    }
    return v;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return vec3_new(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

float random_float() {
    return (float)rand() / (float)RAND_MAX;
}

float random_float_range(float min, float max) {
    return min + (max - min) * random_float();
}
