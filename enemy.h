#pragma once

#include "math.h"
#include "player.h"

#include <SDL3/SDL_render.h>

#define ENEMY_SPEED 10
#define ENEMY_ACCEL 1.1

typedef struct Enemy {
    SDL_Texture *texture;
    SDL_FRect    rect;
    double       speed;
    double       rotation;       // hitbox/draw rotation in degrees
    Vec2d        move_direction; // unit vector
} Enemy;

void Enemy_get_points(const Enemy *this, Vec2d dst[4]);
void Enemy_get_normals(const Enemy *this, Vec2d dst[2]);

int  spawn_enemy(Enemy *dst);
bool collide(Player *player, Enemy *enemy);

