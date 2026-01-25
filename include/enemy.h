#pragma once

#include "my_math.h"
#include "player.h"

#include <SDL3/SDL_render.h>

typedef struct Enemy {
    unsigned int pattern_id;
    SDL_Texture *texture;
    SDL_FRect rect;
    double speed;
    double rotation;      // hitbox/draw rotation in degrees
    Vec2d move_direction; // unit vector
} Enemy;

void Enemy_get_points(const Enemy *this, Vec2d dst[4]);
void Enemy_get_normals(const Enemy *this, Vec2d dst[2]);

///\param spawn the spawn coordinate
///\param spawn_src vector with values from 0% to 100%, the point on the enemy texture which ends up
/// at exact `spawn` coordinate
///\param rotation hitbox/draw rotation in degrees. NAN to look at player
///\param move_direction unit vector of movement direction. use {0,0} to move towards player
int spawn_enemy(Enemy *dst,
                Vec2d spawn,
                Vec2d spawn_src,
                double speed,
                SDL_Color color,
                double rotation,
                Vec2d move_direction,
                unsigned int pattern_id);

int collide(Player *player, Enemy *enemy);

