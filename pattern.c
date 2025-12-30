#include "pattern.h"

#include "enemy.h"
#include "main.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const SDL_Color SDL_COLOR_WHITE = {0xff, 0xff, 0xff, 0xff};

bool should_spawn_enemies(BulletPatternId id, unsigned long ms) {
    static unsigned long last_ms = 0;

#define spawn_if_dt_gt(dt)         \
    {                              \
        if (ms - last_ms > (dt)) { \
            last_ms = ms;          \
            return true;           \
        }                          \
        return false;              \
    }

    switch (id) {
    case Dummy:
        return alive_enemies == 0;
    case SpamTopRight:
        spawn_if_dt_gt(300);
    }
    die("enum");
#undef spawn_if_dt_gt
}

bool should_start_next_pattern(BulletPatternId id, unsigned long time_since_pattern_start_ms) {
    switch (id) {
    case Dummy:
        return time_since_pattern_start_ms > 5000;
    case SpamTopRight:
        return time_since_pattern_start_ms > 2000;
    }
    die("enum");
}

static size_t get_spawn_points(BulletPatternId id, const Vec2d **dst);
static Vec2d  get_spawn_src(BulletPatternId id);
static double get_speed(BulletPatternId id);
static double get_rotation(BulletPatternId id);
static Vec2d  get_move_direction(BulletPatternId id);

void spawn_enemies(BulletPatternId id) {
    const Vec2d *spawns;
    const size_t spawns_len = get_spawn_points(id, &spawns);

    if (alive_enemies + spawns_len > enemies_len) {
        enemies_len = alive_enemies + spawns_len;
        void *ptr   = realloc(enemies, sizeof(enemies[0]) * enemies_len);
        if (!ptr) die("realloc");
        enemies = ptr;
    }

    Vec2d  spawn_src = get_spawn_src(id);
    double speed     = get_speed(id);
    double rotation  = get_rotation(id);

    Vec2d move_direction = get_move_direction(id);

    for (size_t i = 0; i < spawns_len; ++i) {
        const Vec2d *spawn = spawns + i;

        if (!spawn_enemy(enemies + (alive_enemies++),
                         *spawn,
                         spawn_src,
                         speed,
                         SDL_COLOR_WHITE,
                         rotation,
                         move_direction)) {
            break;
        }
    }
}

static size_t get_spawn_points(BulletPatternId id, const Vec2d **const dst) {
    switch (id) {
    case Dummy:
        static Vec2d spawns[] = {{WINDOW_WIDTH, 100}, {WINDOW_WIDTH, 300}, {WINDOW_WIDTH, 600}};
        *dst                  = spawns;
        return size_of_array(spawns);
    case SpamTopRight:
        static Vec2d spawns2[] = {{WINDOW_WIDTH, 0}};
        *dst                   = spawns2;
        return size_of_array(spawns2);
    }
    die("enum");
}

static Vec2d get_spawn_src(BulletPatternId id) {
    switch (id) {
    case Dummy:
        return (Vec2d){1, 1};
    case SpamTopRight:
        return (Vec2d){1, 0};
    }
    die("enum");
}

static double get_speed(BulletPatternId id) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        return 10;
    }
    die("enum");
}
static double get_rotation(BulletPatternId id) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        return NAN;
    }
    die("enum");
}
static Vec2d get_move_direction(BulletPatternId id) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        return (Vec2d){0, 0};
    }
    die("enum");
}

void tick_enemy(BulletPatternId id, Enemy *enemy) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        enemy->speed *= 1.1;
    }
}

