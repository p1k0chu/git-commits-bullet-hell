// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "pattern.hpp"

#include "enemy.hpp"
#include "main.hpp"
#include "utils.hpp"

#include <assert.h>
#include <git2/revwalk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace Patterns {

void BasePattern::tick(unsigned long dt_ms) {
    auto it = enemies.begin();
    while (it != enemies.end()) {
        tick_enemy(*it, dt_ms);
        if (it->can_despawn() && should_despawn(*it)) {
            it = enemies.erase(it);
            continue;
        }
        if (it->collides(player) && !it->is_yellow()) {
            it->make_yellow();
            hit_commits.push_back(it->steal_commit());
        }
        ++it;
    }
    ms_since_start += dt_ms;
}

void BasePattern::tick_enemy(Enemy &enemy, unsigned long dt_ms) {
    enemy.tick(dt_ms);
}

bool BasePattern::should_start_next_pattern() const {
    return walker == nullptr;
}

bool BasePattern::should_despawn(Enemy &enemy) const {
    return !enemy.is_on_screen();
}

void DelaySpawningPattern::tick(unsigned long dt_ms) {
    BasePattern::tick(dt_ms);

    last_spawn_since += dt_ms;
    if (last_spawn_since >= get_delay() && !should_start_next_pattern()) {
        spawn_enemies();
        last_spawn_since = 0;
    }
}

bool TopDown::should_start_next_pattern() const {
    return BasePattern::should_start_next_pattern() || ms_since_start > 10000L;
}

long TopDown::get_delay() const {
    return 150;
}

void TopDown::spawn_enemies() {
    // i love magic numbers /s
    const double c = SDL_cos((float)spawns_counter++ / 3 * 2);
    auto positions = std::array{Vec2d((double)WINDOW_WIDTH / 2.0 - 100.0 * c, 0),
                                Vec2d((double)WINDOW_WIDTH / 2.0 + 100.0 * (1 - c), 0)};
    auto srcs = std::array{Vec2d(1, 0), Vec2d(0, 0)};
    assert(positions.size() == srcs.size());

    for (unsigned long i = 0; i < positions.size(); ++i) {
        auto text = get_next_commit();
        if (text.has_value()) {
            Enemy enemy(std::move(text.value()),
                        positions[i],
                        srcs[i],
                        0,
                        SDL_COLOR_WHITE,
                        {new EnemyBehaviours::MoveInDirection(Vec2d(0, 1), WINDOW_HEIGHT / 4 * 3)});
            enemies.push_back(std::move(enemy));
        } else {
            break;
        }
    }
}

bool Homing::should_start_next_pattern() const {
    return BasePattern::should_start_next_pattern() || spawns_counter >= max_waves;
}

long Homing::get_delay() const {
    return 2000L;
}

void Homing::spawn_enemies() {
    const int counter = this->spawns_counter++;
    int offset = 50;
    for (int i = 0; i < 5; ++i) {
        auto commit = get_next_commit();
        if (!commit.has_value()) {
            return;
        }
        const bool left = (counter / 2) % 2 == 0;
        const bool top = counter % 2 == 0;

        Vec2d anchor{left ? 1 : 0, top ? 0 : 1};
        Vec2d pos{left ? 0 : WINDOW_WIDTH, top ? 0 : WINDOW_HEIGHT};
        Vec2d move_vec{left ? 1 : -1, 0};
        pos.y += top ? offset : -offset;

        Enemy enemy{std::move(commit.value()),
                    pos + (move_vec * 10),
                    anchor,
                    0,
                    SDL_COLOR_WHITE,
                    {new EnemyBehaviours::AppearSlide(pos + (move_vec * (WINDOW_WIDTH / 4)),
                                                      anchor,
                                                      WINDOW_WIDTH / 6),
                     new EnemyBehaviours::AimAtPlayer(1000L),
                     new EnemyBehaviours::MoveWhereEnemyLooks(WINDOW_WIDTH / 2)}};
        offset += enemy.get_dimensions().y + 50;
        enemies.push_back(std::move(enemy));
    }
}

Homing::Homing(long max_waves) : max_waves(max_waves) {}

LeftAndRight::LeftAndRight(int max_waves) : max_waves(max_waves) {}

bool LeftAndRight::should_start_next_pattern() const {
    return waves >= max_waves;
}

long LeftAndRight::get_delay() const {
    return 3000L;
}

void LeftAndRight::spawn_enemies() {
    ++waves;
    int offset = 0;
    bool left = true;
    while (offset < WINDOW_HEIGHT - 1) {
        auto commit = get_next_commit();
        if (!commit.has_value()) {
            return;
        }

        Vec2d anchor{left ? 1 : 0, 0};
        Vec2d pos{left ? 0 : WINDOW_WIDTH, offset};
        Vec2d move_vec{left ? 1 : -1, 0};

        Enemy enemy{std::move(commit.value()),
                    pos + (move_vec * 10),
                    anchor,
                    0,
                    SDL_COLOR_WHITE,
                    {new EnemyBehaviours::MoveInDirection(move_vec, WINDOW_WIDTH / 4)}};
        offset += enemy.get_dimensions().y + player.h - 1;
        enemies.push_back(std::move(enemy));
        left = !left;
    }
}

Scissors::Scissors(int max_waves) : max_waves(max_waves) {}

void Scissors::tick_enemy(Enemy &enemy, unsigned long dt_ms) {
    BasePattern::tick_enemy(enemy, dt_ms);

    Vec2d pos = enemy.get_center();
    double h = enemy.get_dimensions().y;

    if (pos.x < -h) {
        pos.x = WINDOW_WIDTH + h - 1;
    } else if (pos.x > WINDOW_WIDTH + h) {
        pos.x = -h + 1;
    } else {
        return;
    }
    enemy.move_to(pos);
}

bool Scissors::should_start_next_pattern() const {
    return BasePattern::should_start_next_pattern() || waves >= max_waves;
}

bool Scissors::should_despawn(Enemy &enemy) const {
    return enemy.get_center().y > 0 && BasePattern::should_despawn(enemy);
}

long Scissors::get_delay() const {
    return 6000L;
}

void Scissors::spawn_enemies() {
    std::vector<Enemy> line{};
    scissors_spawn_one_line(line, 0.0, true);

    double max_width = -INFINITY;

    auto it = line.begin();
    while (it != line.end()) {
        double w = (it++)->get_dimensions().x;
        if (w > max_width)
            max_width = w;
    }

    assert(!SDL_isinf(max_width));
    assert(!SDL_isnan(max_width));

    while (!line.empty()) {
        Enemy enemy{std::move(line.back())};
        line.pop_back();

        enemy.move(Vec2d(0.0, -max_width));
        enemies.push_back(std::move(enemy));
    }

    scissors_spawn_one_line(enemies, -max_width, false);
}

void Scissors::scissors_spawn_one_line(std::vector<Enemy> &dst, double offset_y, bool align_top) {
    bool to_left = waves++ % 2 == 0;
    Vec2d move_vec = Vec2d::from_angle(SDL_PI_D * (0.50 + (to_left ? 0.1 : -0.1)));
    Vec2d offset{0.0, offset_y};

    while (offset.x < WINDOW_WIDTH - 1) {
        auto commit = get_next_commit();
        if (!commit.has_value()) {
            return;
        }

        Enemy enemy{std::move(commit.value()),
                    offset,
                    Vec2d(0.5, 0.5),
                    90,
                    SDL_COLOR_WHITE,
                    {new EnemyBehaviours::MoveInDirection(move_vec, WINDOW_HEIGHT / 3)}};
        Vec2d dim = enemy.get_dimensions();
        enemy.move(Vec2d(dim.y / 2, (align_top ? 1 : -1) * dim.x / 2 + 1));
        offset.x += dim.y + player.w * 4;
        dst.push_back(std::move(enemy));
    }
}

} // namespace Patterns
