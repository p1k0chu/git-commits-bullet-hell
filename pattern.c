#include "pattern.h"

#include "enemy.h"
#include "main.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const SDL_Color SDL_COLOR_WHITE = {0xff, 0xff, 0xff, 0xff};

bool should_spawn_enemies(BulletPatternId id, unsigned long ms) {
    switch (id) {
    case Dummy:
        return alive_enemies == 0;
    case Dummy2:
        static unsigned long last_ms = 0;

        if (ms - last_ms > 300) {
            last_ms = ms;
            return true;
        }
        return false;
    }
    die("enum");
}

bool should_start_next_pattern(BulletPatternId id, unsigned long time_since_pattern_start_ms) {
    switch (id) {
    case Dummy:
        return time_since_pattern_start_ms > 5000;
    case Dummy2:
        return false;
    }
    die("enum");
}

void spawn_enemies(BulletPatternId id) {
    Vec2d spawn;
    switch (id) {
    case Dummy:
        spawn.x = WINDOW_WIDTH;
        spawn.y = WINDOW_HEIGHT;
        break;
    case Dummy2:
        spawn.x = WINDOW_WIDTH;
        spawn.y = 0;
        break;
    }

    Vec2d spawn_src;
    switch (id) {
    case Dummy:
        spawn_src.x = 1;
        spawn_src.y = 1;
        break;
    case Dummy2:
        spawn_src.x = 1;
        spawn_src.y = 0;
        break;
    }

    double speed;
    switch (id) {
    case Dummy:
    case Dummy2:
        speed = ENEMY_SPEED;
        break;
    }

    double rotation;
    switch (id) {
    case Dummy:
    case Dummy2:
        rotation = NAN;
        break;
    }

    Vec2d move_direction;
    switch (id) {
    case Dummy:
    case Dummy2:
        move_direction.x = 0;
        move_direction.y = 0;
        break;
    }
    spawn_enemy(enemies + (alive_enemies++),
                spawn,
                spawn_src,
                speed,
                SDL_COLOR_WHITE,
                rotation,
                move_direction);
}

