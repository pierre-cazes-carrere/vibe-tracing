#ifndef MATERIAL_H
#define MATERIAL_H

#include "math_utils.h"

typedef enum {
    MAT_DIFFUSE,
    MAT_METAL,
    MAT_DIELECTRIC
} MaterialType;

typedef struct {
    MaterialType type;
    Vec3 albedo;
    float roughness;
    float ior;  // index of refraction for dielectrics
} Material;

Material material_diffuse(Vec3 albedo);
Material material_metal(Vec3 albedo, float roughness);
Material material_dielectric(float ior);

#endif
