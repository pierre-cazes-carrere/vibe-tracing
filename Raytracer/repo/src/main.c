#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600

typedef struct {
    float x, y, z;
} Vec3;

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

int main() {
    printf("Ray Tracer - Version C\n");
    printf("Image: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    Vec3 test = vec3_new(1.0f, 2.0f, 3.0f);
    printf("Test vector: (%.2f, %.2f, %.2f)\n", test.x, test.y, test.z);
    printf("Length: %.2f\n", vec3_length(test));
    
    return 0;
}
