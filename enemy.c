#include "enemy.h"

#include "buffer.h"
#include "main.h"
#include "my_math.h"
#include "utils.h"

#include <SDL3/SDL_stdinc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int spawn_enemy(Enemy *const       dst,
                const Vec2d        spawn,
                const Vec2d        spawn_src,
                const double       speed,
                const SDL_Color    color,
                double             rotation,
                const Vec2d        move_direction,
                const unsigned int pattern_id) {
    if (!has_more_commits) return 0;

    char *line;
    if (!(line = Buffer_get_line(&buffer))) die("Buffer_get_line");

    if (line[0] == 0) {
        has_more_commits = 0;
        return 0;
    }

    dst->speed      = speed;
    dst->pattern_id = pattern_id;

    SDL_Surface *surface = TTF_RenderText_Blended(font, line, 0, color);
    if (!surface) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

    dst->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!dst->texture) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

    SDL_GetTextureSize(dst->texture, &dst->rect.w, &dst->rect.h);

    dst->rect.x = spawn.x - dst->rect.w * spawn_src.x;
    dst->rect.y = spawn.y - dst->rect.h * spawn_src.y;

    double radian = NAN;
    if (SDL_isnan(rotation)) {
        radian = SDL_atan2(player.y - (dst->rect.y + dst->rect.h / 2.0),
                           player.x - (dst->rect.x + dst->rect.w / 2.0));

        rotation = radian * 180.0 / SDL_PI_D;
    }

    while (rotation < 0.0) rotation += 360.0;
    if (rotation > 90.0 && rotation < 270.0) {
        rotation -= 180.0;
    }
    dst->rotation = rotation;

    if (move_direction.x == 0 && move_direction.y == 0) {
        if (SDL_isnan(radian)) {
            radian = SDL_atan2(player.y - (dst->rect.y + dst->rect.h / 2.0),
                               player.x - (dst->rect.x + dst->rect.w / 2.0));
        }
        dst->move_direction.x = SDL_cos(radian);
        dst->move_direction.y = SDL_sin(radian);
    } else {
        dst->move_direction = move_direction;
    }

    return 1;
}

int collide(Player *player, Enemy *enemy) {

    Vec2d normals[4] = {// player's normals
                        {1, 0},
                        {0, 1}};
    Enemy_get_normals(enemy, normals + 2);

    Vec2d player_points[4];
    Player_get_points(player, player_points);

    Vec2d enemy_points[4];
    Enemy_get_points(enemy, enemy_points);

    return polygons_collide(normals, 4, player_points, 4, enemy_points, 4);
}

void Enemy_get_points(const Enemy *this, Vec2d dst[4]) {
    const Vec2d  e_center = {this->rect.x + this->rect.w / 2, this->rect.y + this->rect.h / 2};
    const double rad      = this->rotation * SDL_PI_D / 180.0;

    // top left
    Vec2d tmp = {-this->rect.w / 2, -this->rect.h / 2};
    tmp       = Vec2d_rotate(&tmp, rad);
    dst[0]    = Vec2d_add(&tmp, &e_center);

    // top right
    tmp.x  = this->rect.w / 2;
    tmp.y  = -this->rect.h / 2;
    tmp    = Vec2d_rotate(&tmp, rad);
    dst[1] = Vec2d_add(&tmp, &e_center);

    // bottom left
    tmp.x  = -this->rect.w / 2;
    tmp.y  = this->rect.h / 2;
    tmp    = Vec2d_rotate(&tmp, rad);
    dst[2] = Vec2d_add(&tmp, &e_center);

    // bottom right
    tmp.x  = this->rect.w / 2;
    tmp.y  = this->rect.h / 2;
    tmp    = Vec2d_rotate(&tmp, rad);
    dst[3] = Vec2d_add(&tmp, &e_center);
}

void Enemy_get_normals(const Enemy *this, Vec2d dst[2]) {
    double rad = this->rotation * SDL_PI_D / 180.0;
    dst[0].x   = SDL_cos(rad);
    dst[0].y   = SDL_sin(rad);

    rad += SDL_PI_D / 2;
    dst[1].x = SDL_cos(rad);
    dst[1].y = SDL_sin(rad);
}

