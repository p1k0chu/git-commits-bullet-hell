#pragma once

#include "my_math.h"

#include <SDL3/SDL_render.h>

#define PLAYER_SPEED           400
#define PLAYER_SHIFT_SPEED_MUL .3f
#define PLAYER_HITBOX_MUL      0.5f

typedef struct Player {
    double x;
    double y;
    float  w, h;
    bool   alive;
} Player;

void Player_get_points(const Player *this, Vec2d dst[4]);

