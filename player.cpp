// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "player.hpp"

#include <cassert>
#include <cstdlib>
#include <stdexcept>

Player::Player() : Player(0, 0, 0, 0) {}

Player::Player(double x, double y, float w, float h) : x(x), y(y), w(w), h(h), alive(1) {}

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
