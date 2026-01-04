#include "pattern.h"

#include "enemy.h"
#include "main.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const SDL_Color SDL_COLOR_WHITE = {0xff, 0xff, 0xff, 0xff};

int should_spawn_enemies(BulletPatternId id, unsigned long ms) {
    static unsigned long last_ms = 0;

#define spawn_if_dt_gt(dt)         \
    {                              \
        if (ms - last_ms > (dt)) { \
            last_ms = ms;          \
            return 1;              \
        }                          \
        return 0;                  \
    }

    switch (id) {
    case Dummy:
    case Sides:
        return alive_enemies == 0;
    case SpamTopRight:
        spawn_if_dt_gt(300);
    case TopDown:
        spawn_if_dt_gt(200);
    }
    die("enum");
#undef spawn_if_dt_gt
}

int should_start_next_pattern(BulletPatternId id, unsigned long time_since_pattern_start_ms) {
    switch (id) {
    case Dummy:
        return time_since_pattern_start_ms > 5000;
    case SpamTopRight:
        return time_since_pattern_start_ms > 2000;
    case TopDown:
        return time_since_pattern_start_ms > 2000;
    case Sides:
        return time_since_pattern_start_ms > 2000;
    }
    die("enum");
}

static size_t get_spawn_points(BulletPatternId id, const Vec2d **const dst);
static size_t get_spawn_srcs(BulletPatternId id, const Vec2d **dst);
static double get_speed(BulletPatternId id);
static double get_rotation(BulletPatternId id);
static size_t get_move_direction(BulletPatternId id, const Vec2d **dst);

void spawn_enemies(BulletPatternId id) {
    const Vec2d *spawns;
    const size_t spawns_len = get_spawn_points(id, &spawns);

    if (alive_enemies + spawns_len > enemies_len) {
        enemies_len = alive_enemies + spawns_len;
        void *ptr   = realloc(enemies, sizeof(enemies[0]) * enemies_len);
        if (!ptr) die("realloc");
        enemies = ptr;
    }

    const Vec2d *spawn_srcs;
    const size_t spawn_srcs_len = get_spawn_srcs(id, &spawn_srcs);

    double speed    = get_speed(id);
    double rotation = get_rotation(id);

    const Vec2d *move_directions;
    const size_t move_directions_len = get_move_direction(id, &move_directions);

    size_t i = 0, j = 0, k = 0;
    for (; i < spawns_len; ++i) {
        const Vec2d *spawn          = spawns + i;
        const Vec2d *spawn_src      = spawn_srcs + j;
        const Vec2d *move_direction = move_directions + k;

        if (!spawn_enemy(enemies + alive_enemies,
                         *spawn,
                         *spawn_src,
                         speed,
                         SDL_COLOR_WHITE,
                         rotation,
                         *move_direction,
                         id)) {
            break;
        }
        ++alive_enemies;
        if (j < spawn_srcs_len - 1) ++j;
        if (k < move_directions_len - 1) ++k;
    }
}

static size_t get_spawn_points(BulletPatternId id, const Vec2d **const dst) {

    switch (id) {
    case Dummy:;
        static const Vec2d spawns[] = {{WINDOW_WIDTH, 100},
                                       {WINDOW_WIDTH, 300},
                                       {WINDOW_WIDTH, 600}};
        *dst                        = spawns;
        return size_of_array(spawns);
    case SpamTopRight:;
        static const Vec2d spawns2[] = {{WINDOW_WIDTH, 0}};
        *dst                         = spawns2;
        return size_of_array(spawns2);
    case TopDown:;
        static Vec2d  spawns3[2] = {{0, 0}, {0, 0}};
        static double counter    = 0;

        const double c = SDL_cos(counter);

        spawns3[0].x = (double)WINDOW_WIDTH / 2.0 - 100.0 * c;
        spawns3[1].x = (double)WINDOW_WIDTH / 2.0 + 100.0 * (1 - c);
        counter += 0.3;

        *dst = spawns3;
        return size_of_array(spawns3);
    case Sides:;
        static const Vec2d spawns4[] = {
            // left
            {0, 100},
            {0, 200},
            {0, 400},
            {0, 600},
            {0, 800},
            {0, 1000},

            // right
            {WINDOW_WIDTH, 150},
            {WINDOW_WIDTH, 250},
            {WINDOW_WIDTH, 450},
            {WINDOW_WIDTH, 650},
            {WINDOW_WIDTH, 850},
            {WINDOW_WIDTH, 1500},
        };
        *dst = spawns4;
        return size_of_array(spawns4);
    }
    die("enum");
}

static size_t get_spawn_srcs(BulletPatternId id, const Vec2d **const dst) {
    switch (id) {
    case Dummy:;
        static Vec2d spawns[] = {{1, 0.5}};
        *dst                  = spawns;
        return size_of_array(spawns);
    case SpamTopRight:;
        static Vec2d spawns2[] = {{1, 0}};
        *dst                   = spawns2;
        return size_of_array(spawns2);
    case TopDown:;
        static Vec2d spawns3[] = {{1, 0}, {0, 0}};
        *dst                   = spawns3;
        return size_of_array(spawns3);
    case Sides:;
        static Vec2d spawns4[] = {
            {1, 0.5},
            {1, 0.5},
            {1, 0.5},
            {1, 0.5},
            {1, 0.5},
            {1, 0.5},
            {0, 0.5},
        };
        *dst = spawns4;
        return size_of_array(spawns4);
    }
    die("enum");
}

static double get_speed(BulletPatternId id) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        return 10;
    case TopDown:
        return (double)WINDOW_HEIGHT / 2;
    case Sides:
        return (double)WINDOW_WIDTH / 2;
    }
    die("enum");
}
static double get_rotation(BulletPatternId id) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        return NAN;
    case TopDown:
    case Sides:
        return 0;
    }
    die("enum");
}
static size_t get_move_direction(BulletPatternId id, const Vec2d **dst) {
    switch (id) {
    case Dummy:
    case SpamTopRight:;
        static const Vec2d none = {0, 0};
        *dst                    = &none;
        return 1;
    case TopDown:;
        static const Vec2d down = {0, 1};
        *dst                    = &down;
        return 1;
    case Sides:;
        static const Vec2d array[] = {{1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {-1, 0}};
        *dst                       = array;
        return size_of_array(array);
    }
    die("enum");
}

void tick_enemy(BulletPatternId id, Enemy *enemy) {
    switch (id) {
    case Dummy:
    case SpamTopRight:
        enemy->speed *= 1.1;
        break;
    case TopDown:
    case Sides:
        break;
    }
}

