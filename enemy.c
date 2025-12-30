#include "enemy.h"

#include "buffer.h"
#include "main.h"
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
