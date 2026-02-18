#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "window.h"
#include "renderer.h"
#include "game.h"
#include "math_utils.h"

#define GAME_WIDTH 1024
#define GAME_HEIGHT 768
#define TARGET_FPS 60

int main() {
    srand(time(NULL));
    
    printf("Ray Tracer Game - Real-time Version\n");
    printf("Resolution: %dx%d\n", GAME_WIDTH, GAME_HEIGHT);
    printf("Controls: ZQSD or WASD to move, ESC to quit\n");
    
    // Create window
    GameWindow* window = window_create(GAME_WIDTH, GAME_HEIGHT, "Ray Tracer Game");
    if (!window) {
        printf("Failed to create window\n");
        return 1;
    }
    
    // Create renderer
    Renderer* renderer = renderer_create(GAME_WIDTH, GAME_HEIGHT);
    if (!renderer) {
        printf("Failed to create renderer\n");
        window_free(window);
        return 1;
    }
    
    // Create game
    GameState* game = game_create();
    
    // Camera setup (top-down view centered on player)
    Vec3 camera_pos = vec3_new(0.0f, 3.0f, 0.0f);
    Vec3 camera_dir = vec3_new(0.0f, -1.0f, 0.0f);
    
    // Frame timing
    LARGE_INTEGER frequency, last_count;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last_count);
    
    float frame_time = 1.0f / TARGET_FPS;
    int frame_count = 0;
    float elapsed_time = 0.0f;
    
    printf("\nGame started! Move with ZQSD/WASD, press ESC to quit\n");
    
    // Main game loop
    while (window_is_open(window)) {
        // Calculate delta time
        LARGE_INTEGER current_count;
        QueryPerformanceCounter(&current_count);
        float delta_time = (float)(current_count.QuadPart - last_count.QuadPart) / frequency.QuadPart;
        last_count = current_count;
        
        if (delta_time > 0.05f) delta_time = 0.05f;  // Cap delta time
        
        elapsed_time += delta_time;
        frame_count++;
        
        // Update window events
        window_update(window);
        
        // Handle input (ZQSD or WASD)
        int key_up = window_key_pressed(window, 'Z') || window_key_pressed(window, 'W');
        int key_down = window_key_pressed(window, 'S');
        int key_left = window_key_pressed(window, 'Q') || window_key_pressed(window, 'A');
        int key_right = window_key_pressed(window, 'D');
        
        if (window_key_pressed(window, VK_ESCAPE)) {
            break;
        }
        
        // Update game
        game_handle_input(game, key_up, key_down, key_left, key_right);
        game_update(game, delta_time);
        
        // Render
        renderer_clear(renderer, 0x87CEEB);  // Sky blue background
        renderer_draw_game(renderer, game, camera_pos, camera_dir);
        window_draw_frame(window, renderer->framebuffer);
        
        // Display stats every second
        if (elapsed_time >= 1.0f) {
            printf("FPS: %d | Score: %d | Pos: (%.1f, %.1f, %.1f)\n",
                   frame_count, game->score,
                   game->player_pos.x, game->player_pos.y, game->player_pos.z);
            elapsed_time = 0.0f;
            frame_count = 0;
        }
    }
    
    printf("Game closed. Final score: %d\n", game->score);
    
    // Cleanup
    game_free(game);
    renderer_free(renderer);
    window_free(window);
    
    return 0;
}
