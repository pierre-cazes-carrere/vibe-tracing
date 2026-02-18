#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

GameState* game_create() {
    GameState* game = (GameState*)malloc(sizeof(GameState));
    
    game->player_pos = vec3_new(0.0f, 0.0f, 0.0f);
    game->player_radius = 0.5f;
    game->player_material_id = 0;
    
    game->enemy_count = 3;
    game->enemies = (Enemy*)malloc(game->enemy_count * sizeof(Enemy));
    
    game->enemies[0] = (Enemy){vec3_new(-2.0f, 0.0f, -2.0f), 0.7f, 1, 0.0f, 0.0f};
    game->enemies[1] = (Enemy){vec3_new(2.0f, 0.0f, -2.0f), 0.7f, 1, 0.0f, 0.0f};
    game->enemies[2] = (Enemy){vec3_new(0.0f, 0.0f, -4.0f), 0.6f, 1, 0.0f, 0.0f};
    
    game->collectible_count = 4;
    game->collectibles = (Collectible*)malloc(game->collectible_count * sizeof(Collectible));
    
    game->collectibles[0] = (Collectible){vec3_new(-1.5f, 1.0f, 0.0f), 0.3f, 0};
    game->collectibles[1] = (Collectible){vec3_new(1.5f, 1.0f, 0.0f), 0.3f, 0};
    game->collectibles[2] = (Collectible){vec3_new(0.0f, 0.5f, 1.5f), 0.3f, 0};
    game->collectibles[3] = (Collectible){vec3_new(0.0f, 1.0f, -3.0f), 0.3f, 0};
    
    game->score = 0;
    game->time_elapsed = 0.0f;
    
    return game;
}

void game_free(GameState* game) {
    if (game) {
        free(game->enemies);
        free(game->collectibles);
        free(game);
    }
}

void game_update(GameState* game, float delta_time) {
    game->time_elapsed += delta_time;
    
    // Player movement is now handled by game_handle_input
    
    // Animate enemies (bob up and down)
    for (int i = 0; i < game->enemy_count; i++) {
        game->enemies[i].bob_offset = sinf(game->time_elapsed * 2.0f + i) * 0.3f;
        game->enemies[i].angle = game->time_elapsed * 0.5f;
    }
    
    // Check collectible collisions
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].is_collected) continue;
        
        Vec3 diff = vec3_sub(game->collectibles[i].position, game->player_pos);
        float dist = vec3_length(diff);
        
        if (dist < game->player_radius + game->collectibles[i].radius) {
            game->collectibles[i].is_collected = 1;
            game->score += 10;
        }
    }
}

void game_populate_scene(GameState* game, Scene* scene) {
    // Add ground
    Sphere ground = sphere_create(vec3_new(0.0f, -1.5f, 0.0f), 2.0f, scene->material_count - 1);
    scene_add_object(scene, ground);
    
    // Add player
    Sphere player = sphere_create(game->player_pos, game->player_radius, game->player_material_id);
    scene_add_object(scene, player);
    
    // Add enemies with animation
    for (int i = 0; i < game->enemy_count; i++) {
        Vec3 enemy_pos = game->enemies[i].position;
        enemy_pos.y += game->enemies[i].bob_offset;
        
        Sphere enemy = sphere_create(enemy_pos, game->enemies[i].radius, game->enemies[i].material_id);
        scene_add_object(scene, enemy);
    }
    
    // Add collectibles
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].is_collected) continue;
        
        Sphere collectible = sphere_create(
            game->collectibles[i].position,
            game->collectibles[i].radius,
            3 // gold material
        );
        scene_add_object(scene, collectible);
    }
}

void game_handle_input(GameState* game, int key_up, int key_down, int key_left, int key_right) {
    if (!game) return;
    
    float speed = 3.0f;  // Movement speed
    Vec3 move = vec3_new(0.0f, 0.0f, 0.0f);
    
    // ZQSD or WASD controls
    if (key_up) {
        move.z += speed;  // Move forward
    }
    if (key_down) {
        move.z -= speed;  // Move backward
    }
    if (key_left) {
        move.x -= speed;  // Move left
    }
    if (key_right) {
        move.x += speed;  // Move right
    }
    
    // Clamp player position to bounds
    game->player_pos.x += move.x;
    game->player_pos.z += move.z;
    
    float bound = 3.0f;
    if (game->player_pos.x > bound) game->player_pos.x = bound;
    if (game->player_pos.x < -bound) game->player_pos.x = -bound;
    if (game->player_pos.z > bound) game->player_pos.z = bound;
    if (game->player_pos.z < -bound) game->player_pos.z = -bound;
}
