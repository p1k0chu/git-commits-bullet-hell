// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "player.hpp"

#include "main.hpp"

#include <cassert>
#include <cstdlib>
#include <stdexcept>

Player::Player() : Player(0, 0, 0, 0) {}

Player::Player(double x, double y, float w, float h) : x(x), y(y), w(w), h(h), alive(1) {}

void Player::handle_inputs(char *inputs, unsigned long dt) {
    if (!alive)
        return;

    double speed_mul = (double)dt / 1000.f;

    if (inputs[INPUT_SHIFT] || inputs[INPUT_X]) {
        speed_mul *= PLAYER_SHIFT_SPEED_MUL;
    }

    if (inputs[INPUT_LEFT]) {
        player.x -= PLAYER_SPEED * speed_mul;
        if (player.x < player.w / PLAYER_HITBOX_MUL / 2)
            player.x = player.w / PLAYER_HITBOX_MUL / 2;
    } else if (inputs[INPUT_RIGHT]) {
        player.x += PLAYER_SPEED * speed_mul;
        if (player.x > WINDOW_WIDTH - player.w / PLAYER_HITBOX_MUL / 2)
            player.x = WINDOW_WIDTH - player.w / PLAYER_HITBOX_MUL / 2;
    }

    if (inputs[INPUT_UP]) {
        player.y -= PLAYER_SPEED * speed_mul;
        if (player.y < player.h / PLAYER_HITBOX_MUL / 2)
            player.y = player.h / PLAYER_HITBOX_MUL / 2;
    } else if (inputs[INPUT_DOWN]) {
        player.y += PLAYER_SPEED * speed_mul;
        if (player.y > WINDOW_HEIGHT - player.h / PLAYER_HITBOX_MUL / 2)
            player.y = WINDOW_HEIGHT - player.h / PLAYER_HITBOX_MUL / 2;
    }
}

void Player::get_points(std::span<Vec2d> dst) const {
    assert(dst.size() >= 4);

    const double p_l = this->x - this->w / 2;
    const double p_r = this->x + this->w / 2;
    const double p_t = this->y - this->h / 2;
    const double p_b = this->y + this->h / 2;

    // top left
    dst[0] = Vec2d(p_l, p_t);
    // bottom left
    dst[1] = Vec2d(p_l, p_b);
    // top right
    dst[2] = Vec2d(p_r, p_t);
    // bottom right
    dst[3] = Vec2d(p_r, p_b);
}

Vec2d Player::get_center() const {
    return Vec2d(x + w / 2, y + h / 2);
}

void Player::render(SDL_Renderer *renderer) const {
    SDL_FRect dst{};

    if (alive) {
        SDL_GetTextureSize(player_texture, &dst.w, &dst.h);
        dst.x = player.x - dst.w / 2;
        dst.y = player.y - dst.h / 2;
        SDL_RenderTexture(renderer, player_texture, NULL, &dst);
    } else {
        SDL_GetTextureSize(dead_player_texture, &dst.w, &dst.h);
        dst.x = player.x - dst.w / 2;
        dst.y = player.y - dst.h / 2;
        SDL_RenderTexture(renderer, dead_player_texture, NULL, &dst);
    }
}
