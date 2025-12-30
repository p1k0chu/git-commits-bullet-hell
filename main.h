#pragma once

#include "buffer.h"
#include "enemy.h"
#include "player.h"

#include <SDL3_ttf/SDL_ttf.h>

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

enum Inputs {
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_SHIFT,
    INPUT_X,
};
#define INPUTS_SIZE 6

extern Buffer buffer;

extern SDL_Window   *window;
extern SDL_Renderer *renderer;

extern TTF_Font *font;

extern SDL_Texture *player_texture;
extern SDL_Texture *dead_player_texture;

extern Player player;
extern Enemy  enemies[1];
extern size_t alive_enemies;

extern bool has_more_commits;

extern bool inputs[INPUTS_SIZE];

