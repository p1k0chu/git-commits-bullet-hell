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

    SDL_GetTextureSize(enemies[alive_enemies].texture,
                       &enemies[alive_enemies].rect.w,
                       &enemies[alive_enemies].rect.h);

    enemies[alive_enemies].rect.x = WINDOW_WIDTH - enemies[alive_enemies].rect.w;
    enemies[alive_enemies].rect.y = 0;

    const float radian = SDL_atan2f(
        player.y - (enemies[alive_enemies].rect.y + enemies[alive_enemies].rect.h / 2.0f),
        player.x - (enemies[alive_enemies].rect.x + enemies[alive_enemies].rect.w / 2.0f));
    float degrees = radian * 180.0f / (float)M_PI;
    while (degrees < 0.0f) degrees += 360.0f;

    dst->rotation = degrees;

    return true;
}

bool collide(Player *player, Enemy *enemy) {
    const float enemy_rot = enemy->rotation * M_PI / 180.0f;

    const float enemy_rot_2 = enemy_rot + M_PI_2;

    const Vec2f all_normals[] = {// player's normals
                                 {1, 0},
                                 {0, 1},

                                 // enemy's normals
                                 {cosf(enemy_rot), sinf(enemy_rot)},
                                 {cosf(enemy_rot_2), sinf(enemy_rot_2)}};

    const float p_l = player->x - player->w / 2;
    const float p_r = player->x + player->w / 2;
    const float p_t = player->y - player->h / 2;
    const float p_b = player->y + player->h / 2;

    const Vec2f all_player_points[] = {
        {p_l, p_t}, // top left
        {p_l, p_b}, // bottom left
        {p_r, p_t}, // top right
        {p_r, p_b}  // bottom right
    };

    const Vec2f e_center = {enemy->rect.x + enemy->rect.w / 2, enemy->rect.y + enemy->rect.h / 2};

    Vec2f all_enemy_points[4] = {};

    // top left
    Vec2f tmp           = {-enemy->rect.w / 2, -enemy->rect.h / 2};
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[0] = Vec2f_add(&tmp, &e_center);

    // top right
    tmp.x               = enemy->rect.w / 2;
    tmp.y               = -enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[1] = Vec2f_add(&tmp, &e_center);

    // bottom left
    tmp.x               = -enemy->rect.w / 2;
    tmp.y               = enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[2] = Vec2f_add(&tmp, &e_center);

    // bottom right
    tmp.x               = enemy->rect.w / 2;
    tmp.y               = enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[3] = Vec2f_add(&tmp, &e_center);

    for (size_t i = 0; i < sizeof(all_normals) / sizeof(all_normals[0]); ++i) {
        const Vec2f *normal = all_normals + i;

        float player_min = INFINITY;
        float player_max = -INFINITY;

        for (size_t j = 0; j < sizeof(all_player_points) / sizeof(all_player_points[0]); ++j) {
            const Vec2f *point = all_player_points + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > player_max) player_max = m;
            if (m < player_min) player_min = m;
        }

        float enemy_min = INFINITY;
        float enemy_max = -INFINITY;

        for (size_t j = 0; j < sizeof(all_enemy_points) / sizeof(all_enemy_points[0]); ++j) {
            const Vec2f *point = all_enemy_points + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > enemy_max) enemy_max = m;
            if (m < enemy_min) enemy_min = m;
        }

        if ((player_min < enemy_min && player_max < enemy_min) ||
            (player_min > enemy_max && player_max > enemy_max)) {
            return false;
        }
    }
    return true;
}

