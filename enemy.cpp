// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "enemy.hpp"

#include "main.hpp"
#include "my_math.hpp"
#include "utils.hpp"

#include <SDL3/SDL_stdinc.h>
#include <cassert>
#include <git2/commit.h>
#include <git2/revwalk.h>
#include <math.h>
#include <memory>
#include <span>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

Enemy::Enemy(Wrappers::Commit commit,
             Vec2d position,
             Vec2d spawn_src,
             double rotation,
             SDL_Color color,
             std::initializer_list<EnemyBehaviours::BaseBehaviour *> behaviours) :
    commit(std::move(commit)), rotation(fix_angle(rotation)) {
    assert(!SDL_isnan(rotation));

    this->behaviours.reserve(behaviours.size());
    auto it = behaviours.end() - 1;
    while (it >= behaviours.begin()) {
        this->behaviours.emplace_back(*(it--));
    }

    update_texture(color);

    rect = SDL_FRect{};
    SDL_GetTextureSize(texture, &rect.w, &rect.h);

    rect.x = position.x - rect.w * spawn_src.x;
    rect.y = position.y - rect.h * spawn_src.y;
}

Enemy::~Enemy() {
    SDL_DestroyTexture(texture);
}

Enemy::Enemy(Enemy &&other) :
    texture(other.texture), rect(other.rect), behaviours(std::move(other.behaviours)),
    _is_yellow(other._is_yellow), commit(std::move(other.commit)), rotation(other.rotation) {
    other.texture = nullptr;
}

Enemy &Enemy::operator=(Enemy &&other) {
    if (this != &other) {
        SDL_DestroyTexture(texture);
        texture = other.texture;
        other.texture = nullptr;

        rect = other.rect;
        rotation = other.rotation;
        _is_yellow = other._is_yellow;

        behaviours = std::move(other.behaviours);
        commit = std::move(other.commit);
    }
    return *this;
}

void Enemy::tick(unsigned long dt) {
    if (!behaviours.empty()) {
        EnemyBehaviours::BaseBehaviour *behaviour = behaviours.back().get();
        assert(behaviour != nullptr);
        behaviour->tick(*this, dt);
        if (!behaviour->should_continue(*this)) {
            behaviours.pop_back();
        }
    }
}

void Enemy::get_points(std::span<Vec2d> into) const {
    assert(into.size() >= 4);

    const Vec2d e_center = {this->rect.x + this->rect.w / 2, this->rect.y + this->rect.h / 2};
    const double rad = this->rotation * SDL_PI_D / 180.0;

    // top left
    Vec2d tmp = {-this->rect.w / 2, -this->rect.h / 2};
    tmp = tmp.rotate(rad);
    into[0] = tmp + e_center;

    // top right
    tmp.x = this->rect.w / 2;
    tmp.y = -this->rect.h / 2;
    tmp = tmp.rotate(rad);
    into[1] = tmp + e_center;

    // bottom left
    tmp.x = -this->rect.w / 2;
    tmp.y = this->rect.h / 2;
    tmp = tmp.rotate(rad);
    into[2] = tmp + e_center;

    // bottom right
    tmp.x = this->rect.w / 2;
    tmp.y = this->rect.h / 2;
    tmp = tmp.rotate(rad);
    into[3] = tmp + e_center;
}

void Enemy::get_normals(std::span<Vec2d> into) const {
    assert(into.size() >= 2);

    double rad = deg_to_rad(this->rotation);
    into[0] = Vec2d::from_angle(rad);

    rad += SDL_PI_D / 2;
    into[1] = Vec2d::from_angle(rad);
}

bool Enemy::is_on_screen() const {
    Vec2d normals[4] = {{1, 0}, {0, 1}};
    get_normals(std::span{normals}.subspan(2, 2));

    static const Vec2d screen_points[4] = {{0, 0},
                                           {WINDOW_WIDTH, 0},
                                           {0, WINDOW_HEIGHT},
                                           {WINDOW_WIDTH, WINDOW_HEIGHT}};
    Vec2d enemy_points[4];
    get_points(std::span{enemy_points});

    return polygons_collide(normals, 4, enemy_points, 4, screen_points, 4);
}

bool Enemy::can_despawn() const {
    if (behaviours.empty())
        return true;

    EnemyBehaviours::BaseBehaviour *behaviour = behaviours.back().get();
    assert(behaviour != nullptr);
    return behaviour->can_despawn(*this);
}

bool Enemy::collides(const Player &player) const {
    Vec2d normals[4] = {// player's normals
                        {1, 0},
                        {0, 1}};
    get_normals(std::span{normals}.subspan(2, 2));

    Vec2d player_points[4];
    player.get_points(player_points);

    Vec2d enemy_points[4];
    get_points(std::span{enemy_points});

    return polygons_collide(normals, 4, player_points, 4, enemy_points, 4);
}

void Enemy::render(SDL_Renderer *renderer) const {
    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, rotation, NULL, SDL_FLIP_NONE);
}

Vec2d Enemy::get_center() const {
    return Vec2d(rect.x + rect.w / 2, rect.y + rect.h / 2);
}

Vec2d Enemy::get_position_at(Vec2d anchor) const {
    return Vec2d(rect.x + rect.w * anchor.x, rect.y + rect.h * anchor.y);
}

Vec2d Enemy::get_dimensions() const {
    return Vec2d(rect.w, rect.h);
}

bool Enemy::is_yellow() const {
    return _is_yellow;
}

void Enemy::move(Vec2d delta) {
    rect.x += delta.x;
    rect.y += delta.y;
}

void Enemy::move_to(Vec2d center) {
    rect.x = center.x - rect.w / 2;
    rect.y = center.y - rect.h / 2;
}

void Enemy::move_to(Vec2d position, Vec2d anchor) {
    rect.x = position.x - rect.w * anchor.x;
    rect.y = position.y - rect.h * anchor.y;
}

void Enemy::make_yellow() {
    if (_is_yellow) {
        return;
    }
    update_texture({0xff, 0xff, 0, 0xff});
    _is_yellow = true;
}

Wrappers::Commit Enemy::steal_commit() {
    assert(_is_yellow);
    assert(commit.has_value());
    return std::move(commit);
}

void Enemy::update_texture(SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, commit.summary(), 0, color);
    if (!surface)
        throw SDL_GetError();

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture)
        throw SDL_GetError();
}
