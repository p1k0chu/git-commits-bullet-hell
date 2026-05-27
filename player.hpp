// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#pragma once

#include "my_math.hpp"

#include <SDL3/SDL_render.h>
#include <span>

#define PLAYER_SPEED 400
#define PLAYER_SHIFT_SPEED_MUL .3f
#define PLAYER_HITBOX_MUL 0.5f

struct Player {
    Player();
    Player(double x, double y, float w, float h);

    void handle_inputs(char *inputs, unsigned long dt);

    void get_points(std::span<Vec2d>) const;
    Vec2d get_center() const;
    void render(SDL_Renderer *renderer) const;

    double x, y;
    float w, h;
    char alive;
};

