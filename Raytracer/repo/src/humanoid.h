#ifndef HUMANOID_H
#define HUMANOID_H

#include "math_utils.h"

// Humanoid body parts
typedef struct {
    Vec3 head_pos;
    Vec3 torso_pos;
    Vec3 left_arm_pos;
    Vec3 right_arm_pos;
    Vec3 left_leg_pos;
    Vec3 right_leg_pos;
    
    float head_radius;
    float limb_scale;  // Scale factor for arms/legs
    
    Vec3 position;     // Center position
    Vec3 direction;    // Facing direction
    float animation_time;  // For walking/movement animation
} Humanoid;

// Initialize a humanoid at a position
Humanoid humanoid_create(Vec3 pos, Vec3 dir);

// Update humanoid pose based on movement and time
void humanoid_update(Humanoid* h, Vec3 movement_dir, float delta_time);

// Get the positions of all body parts (called internally for rendering)
void humanoid_compute_parts(Humanoid* h);

#endif
