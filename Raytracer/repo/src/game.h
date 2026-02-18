#ifndef GAME_H
#define GAME_H

#include "math_utils.h"
#include "raytracer.h"

typedef struct {
    Vec3 position;
    float radius;
    int is_active;
} GameObject;

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
    Vec3 player_pos;
    float player_radius;
    int player_material_id;
    
    Enemy* enemies;
    int enemy_count;
    
    Collectible* collectibles;
    int collectible_count;
    
    int score;
    float time_elapsed;
} GameState;

GameState* game_create();
void game_free(GameState* game);
void game_update(GameState* game, float delta_time);
void game_populate_scene(GameState* game, Scene* scene);

#endif
