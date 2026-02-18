#include "material.h"

Material material_diffuse(Vec3 albedo) {
    return (Material){
        .type = MAT_DIFFUSE,
        .albedo = albedo,
        .roughness = 1.0f,
        .ior = 1.0f
    };
}

Material material_metal(Vec3 albedo, float roughness) {
    return (Material){
        .type = MAT_METAL,
        .albedo = albedo,
        .roughness = roughness,
        .ior = 1.0f
    };
}

Material material_dielectric(float ior) {
    return (Material){
        .type = MAT_DIELECTRIC,
        .albedo = (Vec3){1.0f, 1.0f, 1.0f},
        .roughness = 0.0f,
        .ior = ior
    };
}
