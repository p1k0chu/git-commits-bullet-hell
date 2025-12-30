#include "enemy.h"

#include "buffer.h"
#include "main.h"
#include "math.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int spawn_enemy(Enemy *dst) {
    if (!has_more_commits) return false;

    char *line;
    if (!(line = Buffer_get_line(&buffer))) die("Buffer_get_line");

    if (line[0] == 0) {
        has_more_commits = false;
        return true;
    }

    dst->speed = ENEMY_SPEED;

    const SDL_Color white_color = {0xff, 0xff, 0xff, 0xff};

    SDL_Surface *surface = TTF_RenderText_Blended(font, line, 0, white_color);
    if (!surface) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

    dst->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!dst->texture) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

    SDL_GetTextureSize(dst->texture, &dst->rect.w, &dst->rect.h);

    dst->rect.x = WINDOW_WIDTH - dst->rect.w;
    dst->rect.y = 0;

    const double radian  = SDL_atan2(player.y - (dst->rect.y + dst->rect.h / 2.0),
                                    player.x - (dst->rect.x + dst->rect.w / 2.0));
    double       degrees = radian * 180.0 / (double)M_PI;
    while (degrees < 0.0) degrees += 360.0;

    if (degrees > 90.0 && degrees < 270.0) {
        degrees -= 180.0;
    }
    dst->rotation = degrees;

    dst->move_direction.x = cos(radian);
    dst->move_direction.y = sin(radian);

    return true;
}

bool collide(Player *player, Enemy *enemy) {

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
    const double rad      = this->rotation * M_PI / 180.0;

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
    double rad = this->rotation * M_PI / 180.0;
    dst[0].x   = cos(rad);
    dst[0].y   = sin(rad);

    rad += M_PI_2;
    dst[1].x = cos(rad);
    dst[1].y = sin(rad);
}

