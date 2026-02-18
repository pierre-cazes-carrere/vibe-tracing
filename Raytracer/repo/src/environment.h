#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "math_utils.h"

// Architectural structure types
typedef enum {
    ARCH_COLUMN,
    ARCH_WALL,
    ARCH_ARCH,
    ARCH_TOWER,
    ARCH_RUIN
} ArchitectureType;

// 3D structure
typedef struct {
    Vec3 position;
    Vec3 size;
    ArchitectureType type;
    float rotation_y;
    int is_destroyed;
} Structure;

// Environment for gothic/magical atmosphere
typedef struct {
    Structure* structures;
    int structure_count;
    int structure_capacity;
    
    // Atmosphere parameters
    float fog_density;      // For atmospheric effects
    float time_of_day;      // For lighting changes
    int theme;              // 0=gothic, 1=magical, 2=corrupted
} Environment;

Environment* environment_create();
void environment_free(Environment* env);
void environment_add_structure(Environment* env, Vec3 pos, Vec3 size, ArchitectureType type);
void environment_update(Environment* env, float delta_time);
void environment_populate_gothic_arena(Environment* env);

#endif
