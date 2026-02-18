#include "environment.h"
#include <stdlib.h>
#include <math.h>

Environment* environment_create() {
    Environment* env = (Environment*)malloc(sizeof(Environment));
    env->structure_capacity = 32;
    env->structure_count = 0;
    env->structures = (Structure*)malloc(env->structure_capacity * sizeof(Structure));
    env->fog_density = 0.3f;
    env->time_of_day = 18.0f;  // Evening for gothic atmosphere
    env->theme = 0;  // Gothic
    
    return env;
}

void environment_free(Environment* env) {
    if (env) {
        free(env->structures);
        free(env);
    }
}

void environment_add_structure(Environment* env, Vec3 pos, Vec3 size, ArchitectureType type) {
    if (!env || env->structure_count >= env->structure_capacity) return;
    
    Structure s = {pos, size, type, 0.0f, 0};
    env->structures[env->structure_count++] = s;
}

void environment_update(Environment* env, float delta_time) {
    if (!env) return;
    
    // Animate time of day for atmospheric effects
    env->time_of_day += delta_time * 0.1f;
    if (env->time_of_day >= 24.0f) env->time_of_day -= 24.0f;
}

void environment_populate_gothic_arena(Environment* env) {
    if (!env) return;
    
    // Clear existing structures
    env->structure_count = 0;
    
    // Central gothic tower
    environment_add_structure(env, vec3_new(0.0f, 0.0f, 0.0f), vec3_new(1.5f, 3.0f, 1.5f), ARCH_TOWER);
    
    // Surrounding pillars (classic gothic architecture)
    float pillar_radius = 3.0f;
    for (int i = 0; i < 8; i++) {
        float angle = (i / 8.0f) * 6.28318f;  // 2*PI
        float x = cosf(angle) * pillar_radius;
        float z = sinf(angle) * pillar_radius;
        
        environment_add_structure(env, vec3_new(x, 0.0f, z), vec3_new(0.6f, 2.5f, 0.6f), ARCH_COLUMN);
    }
    
    // Connecting arches between pillars
    for (int i = 0; i < 4; i++) {
        float angle1 = (i / 4.0f) * 6.28318f;
        float angle2 = ((i + 1) / 4.0f) * 6.28318f;
        
        float x1 = cosf(angle1) * pillar_radius;
        float z1 = sinf(angle1) * pillar_radius;
        float x2 = cosf(angle2) * pillar_radius;
        float z2 = sinf(angle2) * pillar_radius;
        
        float mid_x = (x1 + x2) * 0.5f;
        float mid_z = (z1 + z2) * 0.5f;
        float dist = sqrtf((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
        
        environment_add_structure(env, vec3_new(mid_x, 1.0f, mid_z), 
                                 vec3_new(0.3f, 0.5f, dist), ARCH_ARCH);
    }
    
    // Outer walls (ruined castle walls)
    float wall_radius = 6.0f;
    for (int i = 0; i < 4; i++) {
        float angle = (i / 4.0f) * 6.28318f;
        float x = cosf(angle) * wall_radius;
        float z = sinf(angle) * wall_radius;
        
        environment_add_structure(env, vec3_new(x, 0.0f, z), vec3_new(0.4f, 2.0f, 0.4f), ARCH_RUIN);
    }
}
