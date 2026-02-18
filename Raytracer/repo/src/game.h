#ifndef GAME_H
#define GAME_H

#include "math_utils.h"
#include "raytracer.h"
#include "environment.h"

typedef struct {
    Vec3 position;
    Vec3 velocity;
    float lifetime;
    int is_active;
} Projectile;

typedef struct {
    Vec3 position;
    float radius;
    int material_id;
    float angle;
    float bob_offset;
} Enemy;

typedef struct {
    Vec3 position;
    float radius;
    int is_collected;
} Collectible;

typedef struct {
    Vec3 position;
    Vec3 direction;  // facing direction (normalized)
    float radius;
    int material_id;
    float weapon_cooldown;
} Player;

typedef struct {
    Player player;
    
    Enemy* enemies;
    int enemy_count;
    
    Projectile* projectiles;
    int projectile_count;
    int projectile_capacity;
    
    Environment* environment;  // Gothic environment with structures
    
    Collectible* collectibles;
    int collectible_count;
    
    int score;
    float time_elapsed;
} GameState;

GameState* game_create();
void game_free(GameState* game);
void game_update(GameState* game, float delta_time);
void game_populate_scene(GameState* game, Scene* scene);
void game_handle_input(GameState* game, int key_up, int key_down, int key_left, int key_right, int fire_weapon);

#endif
