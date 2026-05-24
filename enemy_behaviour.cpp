#include "enemy_behaviour.hpp"

#include "enemy.hpp"
#include "main.hpp"

namespace EnemyBehaviours {

void BaseBehaviour::tick(Enemy &enemy, unsigned long dt_ms) {
    alive_ms += dt_ms;
}

bool BaseBehaviour::should_continue(Enemy &enemy) const {
    return true;
}

bool BaseBehaviour::can_despawn(const Enemy &enemy) const {
    return true;
}

MoveInDirection::MoveInDirection(Vec2d move_direction, double speed) :
    move_direction(move_direction), speed(speed) {}

void MoveInDirection::tick(Enemy &enemy, unsigned long dt_ms) {
    BaseBehaviour::tick(enemy, dt_ms);

    enemy.move(move_direction * (speed * (double)dt_ms / 1000.f));
}

AppearSlide::AppearSlide(Vec2d goal_position, double speed) :
    AppearSlide(goal_position, Vec2d(), speed) {}

AppearSlide::AppearSlide(Vec2d goal_position, Vec2d anchor, double speed) :
    goal_position(goal_position), anchor(anchor), speed(speed) {}

void AppearSlide::tick(Enemy &enemy, unsigned long dt_ms) {
    BaseBehaviour::tick(enemy, dt_ms);

    Vec2d pos = enemy.get_position_at(anchor);
    Vec2d direction = pos.look_at(goal_position);
    Vec2d move_vec = direction * (speed * (double)dt_ms / 1000.f);
    if (move_vec.magnitude_sqr() >= (goal_position - pos).magnitude_sqr()) {
        enemy.move_to(goal_position, anchor);
    } else {
        enemy.move(move_vec);
    }
}

bool AppearSlide::should_continue(Enemy &enemy) const {
    return enemy.get_position_at(anchor) != goal_position;
}

AimAtPlayer::AimAtPlayer(unsigned long duration) : duration(duration) {}

void AimAtPlayer::tick(Enemy &enemy, unsigned long dt_ms) {
    BaseBehaviour::tick(enemy, dt_ms);

    const double goal_angle = enemy.get_center().look_at(player.get_center()).angle_deg();
    const double delta = fix_angle(goal_angle) - enemy.rotation;
    enemy.rotation += delta * 0.3;
}

bool AimAtPlayer::should_continue(Enemy &enemy) const {
    return alive_ms <= duration;
}

bool AimAtPlayer::can_despawn(const Enemy &enemy) const {
    return false;
}

MoveWhereEnemyLooks::MoveWhereEnemyLooks(double speed) : speed(speed) {}

void MoveWhereEnemyLooks::tick(Enemy &enemy, unsigned long dt_ms) {
    BaseBehaviour::tick(enemy, dt_ms);

    enemy.move(Vec2d::from_angle(deg_to_rad(enemy.rotation)) * ((double)dt_ms / 1000.f * speed));
}

} // namespace EnemyBehaviours

