#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

GameState* game_create() {
    GameState* game = (GameState*)malloc(sizeof(GameState));
    
    // Initialize player
    game->player.position = vec3_new(0.0f, 0.0f, 8.0f);
    game->player.direction = vec3_new(0.0f, 0.0f, -1.0f);
    game->player.radius = 0.4f;
    game->player.material_id = 0;
    game->player.weapon_cooldown = 0.0f;
    
    // Initialize enemies - larger exploration area
    game->enemy_count = 5;
    game->enemies = (Enemy*)malloc(game->enemy_count * sizeof(Enemy));
    game->enemies[0] = (Enemy){vec3_new(-3.0f, 0.0f, -3.0f), 0.5f, 1, 0.0f, 0.0f};
    game->enemies[1] = (Enemy){vec3_new(3.0f, 0.0f, -3.0f), 0.5f, 1, 0.0f, 0.0f};
    game->enemies[2] = (Enemy){vec3_new(0.0f, 0.0f, -6.0f), 0.5f, 1, 0.0f, 0.0f};
    game->enemies[3] = (Enemy){vec3_new(-4.0f, 0.0f, 2.0f), 0.5f, 1, 0.0f, 0.0f};
    game->enemies[4] = (Enemy){vec3_new(4.0f, 0.0f, 2.0f), 0.5f, 1, 0.0f, 0.0f};
    
    // Initialize projectiles
    game->projectile_capacity = 50;
    game->projectile_count = 0;
    game->projectiles = (Projectile*)malloc(game->projectile_capacity * sizeof(Projectile));
    
    // Initialize gothic environment
    game->environment = environment_create();
    environment_populate_gothic_arena(game->environment);
    
    // Initialize collectibles - removed, less important
    game->collectible_count = 0;
    game->collectibles = NULL;
    
    game->score = 0;
    game->time_elapsed = 0.0f;
    
    return game;
}

void game_free(GameState* game) {
    if (game) {
        free(game->enemies);
        free(game->projectiles);
        if (game->collectibles) free(game->collectibles);
        environment_free(game->environment);
        free(game);
    }
}


void game_update(GameState* game, float delta_time) {
    game->time_elapsed += delta_time;
    
    // Update environment (time of day, atmosphere)
    if (game->environment) {
        environment_update(game->environment, delta_time);
    }
    
    // Update weapon cooldown
    if (game->player.weapon_cooldown > 0.0f) {
        game->player.weapon_cooldown -= delta_time;
    }
    
    // Update enemies - make them patrol/turn towards player
    for (int i = 0; i < game->enemy_count; i++) {
        // Bobbing animation
        game->enemies[i].bob_offset = sinf(game->time_elapsed * 2.0f + i) * 0.2f;
        game->enemies[i].angle = game->time_elapsed * 0.3f;
        
        // Simple patrol - move in circles
        game->enemies[i].position.x += cosf(game->time_elapsed * 0.5f + i * 1.256f) * 0.3f * delta_time;
        game->enemies[i].position.z += sinf(game->time_elapsed * 0.5f + i * 1.256f) * 0.3f * delta_time;
    }
    
    // Update projectiles
    for (int i = 0; i < game->projectile_count; i++) {
        game->projectiles[i].position = vec3_add(game->projectiles[i].position,
            vec3_mul(game->projectiles[i].velocity, delta_time));
        game->projectiles[i].lifetime -= delta_time;
        
        if (game->projectiles[i].lifetime <= 0.0f) {
            game->projectiles[i].is_active = 0;
        }
        
        // Check collisions with enemies
        for (int e = 0; e < game->enemy_count; e++) {
            if (!game->enemies[e].radius) continue;
            
            Vec3 diff = vec3_sub(game->projectiles[i].position, game->enemies[e].position);
            float dist = vec3_length(diff);
            
            if (dist < game->enemies[e].radius + 0.1f) {
                game->projectiles[i].is_active = 0;
                game->projectiles[i].lifetime = 0.0f;
                game->enemies[e].radius = 0.0f;  // Mark as dead
                game->score += 100;
            }
        }
    }
    
    // Remove inactive projectiles
    int write_idx = 0;
    for (int i = 0; i < game->projectile_count; i++) {
        if (game->projectiles[i].is_active) {
            game->projectiles[write_idx++] = game->projectiles[i];
        }
    }
    game->projectile_count = write_idx;
}


void game_populate_scene(GameState* game, Scene* scene) {
    // Add ground
    Sphere ground = sphere_create(vec3_new(0.0f, -1.5f, 0.0f), 5.0f, scene->material_count - 1);
    scene_add_object(scene, ground);
    
    // Add player
    Sphere player = sphere_create(game->player.position, game->player.radius, game->player.material_id);
    scene_add_object(scene, player);
    
    // Add enemies
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].radius <= 0.0f) continue;  // Skip dead enemies
        
        Vec3 enemy_pos = game->enemies[i].position;
        enemy_pos.y += game->enemies[i].bob_offset;
        
        Sphere enemy = sphere_create(enemy_pos, game->enemies[i].radius, game->enemies[i].material_id);
        scene_add_object(scene, enemy);
    }
}


void game_handle_input(GameState* game, int key_up, int key_down, int key_left, int key_right, int fire_weapon) {
    if (!game || !game->environment) return;
    
    float speed = 5.0f;  // Movement speed
    Vec3 move = vec3_new(0.0f, 0.0f, 0.0f);
    
    // ZQSD or WASD controls
    if (key_up) {
        move.z -= speed;  // Move forward (negative Z)
    }
    if (key_down) {
        move.z += speed;  // Move backward (positive Z)
    }
    if (key_left) {
        move.x -= speed;  // Move left
    }
    if (key_right) {
        move.x += speed;  // Move right
    }
    
    // Update direction based on movement
    if (key_up || key_down || key_left || key_right) {
        game->player.direction = vec3_normalize(move);
    }
    
    // Apply movement with collision
    Vec3 new_pos = vec3_add(game->player.position, vec3_mul(move, 1.0f/60.0f));  // Assuming 60FPS
    
    // Simple boundary clamping extended for exploration
    float bound = 5.5f;
    if (new_pos.x > bound) new_pos.x = bound;
    if (new_pos.x < -bound) new_pos.x = -bound;
    if (new_pos.z > bound) new_pos.z = bound;
    if (new_pos.z < -bound) new_pos.z = -bound;
    
    // Collision detection with environmental structures
    int collision = 0;
    for (int i = 0; i < game->environment->structure_count; i++) {
        Structure* s = &game->environment->structures[i];
        Vec3 half_size = vec3_mul(s->size, 0.5f);
        float dx = fabsf(new_pos.x - s->position.x);
        float dz = fabsf(new_pos.z - s->position.z);
        
        if (dx < half_size.x + game->player.radius && dz < half_size.z + game->player.radius) {
            collision = 1;
            break;
        }
    }
    
    if (!collision) {
        game->player.position = new_pos;
    }
    
    // Fire weapon
    if (fire_weapon && game->player.weapon_cooldown <= 0.0f && game->projectile_count < game->projectile_capacity) {
        Vec3 projectile_pos = vec3_add(game->player.position, vec3_mul(game->player.direction, 0.5f));
        Vec3 projectile_vel = vec3_mul(game->player.direction, 15.0f);  // 15 units/sec
        
        Projectile proj = (Projectile){
            projectile_pos,
            projectile_vel,
            3.0f,  // 3 second lifetime
            1
        };
        
        game->projectiles[game->projectile_count++] = proj;
        game->player.weapon_cooldown = 0.2f;  // 200ms cooldown between shots
    }
}
