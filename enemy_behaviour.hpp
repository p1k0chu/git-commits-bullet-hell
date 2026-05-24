#pragma once

#include "my_math.hpp"

class Enemy;

namespace EnemyBehaviours {

class BaseBehaviour {
  public:
    virtual ~BaseBehaviour() = default;

    virtual void tick(Enemy& enemy, unsigned long dt_ms);

    virtual bool should_continue(Enemy& enemy) const;
    virtual bool can_despawn(const Enemy& enemy) const;

  protected:
    unsigned long alive_ms = 0;
};

class MoveInDirection : public BaseBehaviour {
  public:
    MoveInDirection(Vec2d move_direction, double speed);

    void tick(Enemy& enemy, unsigned long dt_ms) override;

  private:
    Vec2d move_direction;
    double speed;
};

class MoveWhereEnemyLooks : public BaseBehaviour {
  public:
    MoveWhereEnemyLooks(double speed);

    void tick(Enemy& enemy, unsigned long dt_ms) override;

  private:
    double speed;
};

class AppearSlide : public BaseBehaviour {
  public:
    AppearSlide(Vec2d goal_position, double speed);
    AppearSlide(Vec2d goal_position, Vec2d anchor, double speed);

    void tick(Enemy& enemy, unsigned long dt_ms) override;

    bool should_continue(Enemy& enemy) const override;

  private:
    Vec2d goal_position;
    Vec2d anchor;
    double speed;
};

class AimAtPlayer : public BaseBehaviour {
  public:
    AimAtPlayer(unsigned long duration);

    void tick(Enemy& enemy, unsigned long dt_ms) override;

    bool should_continue(Enemy& enemy) const override;
    bool can_despawn(const Enemy& enemy) const override;

  private:
    unsigned long duration;
};

} // namespace EnemyBehaviours

