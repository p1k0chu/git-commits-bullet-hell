#pragma once

#include <SDL3/SDL_render.h>

#define PLAYER_SPEED           400
#define PLAYER_SHIFT_SPEED_MUL .3f

typedef struct Player {
    float x;
    float y;
    bool  alive;
} Player;

