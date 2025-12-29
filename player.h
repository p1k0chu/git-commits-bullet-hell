#pragma once

#include <SDL3/SDL_render.h>

#define PLAYER_SPEED 100

typedef struct Player {
    float x;
    float y;
    bool  alive;
} Player;

