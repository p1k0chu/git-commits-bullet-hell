#pragma once

#include <SDL3/SDL_render.h>

#define ENEMY_SPEED 10
#define ENEMY_ACCEL 1.1

typedef struct Enemy {
    SDL_Texture *texture;
    SDL_FRect    rect;
    float        speed;
    float        rotation;
} Enemy;

