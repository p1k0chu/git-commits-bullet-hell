// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#pragma once

#include "enemy.hpp"
#include "player.hpp"
#include "wrappers.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <git2/types.h>
#include <vector>

#define WINDOW_WIDTH 1920
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

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern TTF_Font *font;

extern SDL_Texture *player_texture;
extern SDL_Texture *dead_player_texture;

extern Player player;

extern git_repository *repo;
extern git_revwalk *walker;

extern std::vector<Wrappers::Commit> hit_commits;

extern char inputs[INPUTS_SIZE];
extern char started;

