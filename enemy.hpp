// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#pragma once

#include "enemy_behaviour.hpp"
#include "my_math.hpp"
#include "player.hpp"

#include <SDL3/SDL_render.h>
#include <memory>
#include <span>
#include <string>
#include <vector>

class Enemy {
  public:
    Enemy(std::string text,
          Vec2d position,
          Vec2d spawn_src,
          double rotation,
          SDL_Color color,
          std::initializer_list<EnemyBehaviours::BaseBehaviour*> behaviours);
    ~Enemy();

    Enemy(const Enemy&) = delete;
    Enemy& operator=(const Enemy&) = delete;

    Enemy(Enemy&&);
    Enemy& operator=(Enemy&& other);

    void tick(unsigned long dt);
    void move(Vec2d delta);
    void move_to(Vec2d center);
    void move_to(Vec2d position, Vec2d anchor);

    void get_points(std::span<Vec2d> into) const;
    void get_normals(std::span<Vec2d> into) const;
    bool is_on_screen() const;
    bool can_despawn() const;
    bool collides(const Player& player) const;
    void render(SDL_Renderer* renderer) const;
    Vec2d get_center() const;
    Vec2d get_position_at(Vec2d anchor) const;
    Vec2d get_dimensions() const;

  private:
    SDL_Texture* texture;
    SDL_FRect rect;
    std::vector<std::unique_ptr<EnemyBehaviours::BaseBehaviour>> behaviours;

  public:
    double rotation;
};
