#include "humanoid.h"
#include <math.h>

Humanoid humanoid_create(Vec3 pos, Vec3 dir) {
    Humanoid h;
    h.position = pos;
    h.direction = vec3_normalize(dir);
    h.animation_time = 0.0f;
    h.head_radius = 0.3f;
    h.limb_scale = 0.15f;
    
    humanoid_compute_parts(&h);
    return h;
}

void humanoid_update(Humanoid* h, Vec3 movement_dir, float delta_time) {
    if (!h) return;
    
    h->animation_time += delta_time;
    
    // Update direction based on movement
    if (movement_dir.x != 0.0f || movement_dir.z != 0.0f) {
        h->direction = vec3_normalize(movement_dir);
    }
    
    humanoid_compute_parts(h);
}

void humanoid_compute_parts(Humanoid* h) {
    if (!h) return;
    
    // Base position offsets relative to center
    // Head is above the torso
    h->head_pos = vec3_add(h->position, vec3_new(0.0f, 0.5f, 0.0f));
    
    // Torso is at center
    h->torso_pos = h->position;
    
    // Walking animation - legs swing back and forth
    float walk_cycle = sinf(h->animation_time * 4.0f);  // Faster animation
    float walk_amplitude = 0.1f;
    
    // Left leg swings forward when right leg is back
    h->left_leg_pos = vec3_add(h->position, 
        vec3_new(-0.15f, -0.35f, walk_cycle * walk_amplitude));
    
    // Right leg is opposite phase
    h->right_leg_pos = vec3_add(h->position,
        vec3_new(0.15f, -0.35f, -walk_cycle * walk_amplitude));
    
    // Arms swing opposite to legs for natural motion
    float arm_swing = -walk_cycle * 0.08f;
    
    // Left arm
    h->left_arm_pos = vec3_add(h->position,
        vec3_new(-0.25f, 0.1f, arm_swing));
    
    // Right arm
    h->right_arm_pos = vec3_add(h->position,
        vec3_new(0.25f, 0.1f, -arm_swing));
}
