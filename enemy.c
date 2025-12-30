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

    const float radian  = SDL_atan2f(player.y - (dst->rect.y + dst->rect.h / 2.0f),
                                    player.x - (dst->rect.x + dst->rect.w / 2.0f));
    float       degrees = radian * 180.0f / (float)M_PI;
    while (degrees < 0.0f) degrees += 360.0f;

    if (degrees > 90.0f && degrees < 270.0f) {
        degrees -= 180.0f;
    }
    dst->rotation = degrees;

    dst->move_direction.x = cosf(radian);
    dst->move_direction.y = sinf(radian);

    return true;
}

bool collide(Player *player, Enemy *enemy) {

    Vec2f normals[4] = {// player's normals
                        {1, 0},
                        {0, 1}};
    Enemy_get_normals(enemy, normals + 2);

    Vec2f player_points[4];
    Player_get_points(player, player_points);

    Vec2f enemy_points[4];
    Enemy_get_points(enemy, enemy_points);

    return polygons_collide(normals, 4, player_points, 4, enemy_points, 4);
}

void Enemy_get_points(const Enemy *this, Vec2f dst[4]) {
    const Vec2f e_center = {this->rect.x + this->rect.w / 2, this->rect.y + this->rect.h / 2};
    const float rad      = this->rotation * M_PI / 180.0f;

    // top left
    Vec2f tmp = {-this->rect.w / 2, -this->rect.h / 2};
    tmp       = Vec2f_rotate(&tmp, rad);
    dst[0]    = Vec2f_add(&tmp, &e_center);

    // top right
    tmp.x  = this->rect.w / 2;
    tmp.y  = -this->rect.h / 2;
    tmp    = Vec2f_rotate(&tmp, rad);
    dst[1] = Vec2f_add(&tmp, &e_center);

    // bottom left
    tmp.x  = -this->rect.w / 2;
    tmp.y  = this->rect.h / 2;
    tmp    = Vec2f_rotate(&tmp, rad);
    dst[2] = Vec2f_add(&tmp, &e_center);

    // bottom right
    tmp.x  = this->rect.w / 2;
    tmp.y  = this->rect.h / 2;
    tmp    = Vec2f_rotate(&tmp, rad);
    dst[3] = Vec2f_add(&tmp, &e_center);
}

void Enemy_get_normals(const Enemy *this, Vec2f dst[2]) {
    float rad = this->rotation * M_PI / 180.0f;
    dst[0].x  = cosf(rad);
    dst[0].y  = sinf(rad);

    rad += M_PI_2;
    dst[0].x = cosf(rad);
    dst[0].y = sinf(rad);
}

