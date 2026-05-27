#pragma once

#include "enemy.hpp"
#include "my_math.hpp"

#include <vector>

namespace Patterns {

static const SDL_Color SDL_COLOR_WHITE = {0xff, 0xff, 0xff, 0xff};
static const SDL_Color SDL_COLOR_YELLOW = {0xff, 0xff, 0x00, 0xff};

class BasePattern {
  public:
    std::vector<Enemy> enemies;

    virtual ~BasePattern() = default;

    virtual bool should_start_next_pattern() const;
    virtual bool should_despawn(Enemy& enemy) const;

    virtual void tick(unsigned long dt_ms);
    virtual void tick_enemy(Enemy& enemy, unsigned long dt_ms);

  protected:
    unsigned long ms_since_start;
};

class DelaySpawningPattern : public BasePattern {
  public:
    virtual void tick(unsigned long dt_ms) override;

  protected:
    virtual long get_delay() const = 0;
    virtual void spawn_enemies() = 0;

  private:
    long last_spawn_since = 99999L;
};

class TopDown : public DelaySpawningPattern {
  public:
    bool should_start_next_pattern() const override;

  protected:
    long get_delay() const override;
    void spawn_enemies() override;

  private:
    long spawns_counter = 0;
};

class Homing : public DelaySpawningPattern {
  public:
    Homing(long max_waves);

    bool should_start_next_pattern() const override;

  protected:
    long get_delay() const override;
    void spawn_enemies() override;

  private:
    long spawns_counter = 0;
    long max_waves;
};

class LeftAndRight : public DelaySpawningPattern {
  public:
    LeftAndRight(int max_waves);

    bool should_start_next_pattern() const override;

  protected:
    long get_delay() const override;
    void spawn_enemies() override;

  private:
    int waves = 0;
    int max_waves;
};

class Scissors : public DelaySpawningPattern {
    // yes i couldn't come up with a better name im sorry

  public:
    Scissors(int max_waves);

    void tick_enemy(Enemy& enemy, unsigned long dt_ms) override;

    bool should_start_next_pattern() const override;
    bool should_despawn(Enemy& enemy) const override;

  protected:
    long get_delay() const override;
    void spawn_enemies() override;
    void scissors_spawn_one_line(std::vector<Enemy>& dst, double offset_y, bool align_top);

  private:
    int waves = 0;
    int max_waves;
};

} // namespace Patterns
