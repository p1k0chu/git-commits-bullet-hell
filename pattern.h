#pragma once

#include "enemy.h"

typedef enum BulletPatternId {
    Dummy,
    SpamTopRight,
    TopDown,
} BulletPatternId;

#define BULLET_PATTERN_ID_LEN 3

bool should_spawn_enemies(BulletPatternId, unsigned long time_ms);
bool should_start_next_pattern(BulletPatternId, unsigned long time_since_pattern_start_ms);
void spawn_enemies(BulletPatternId);
void tick_enemy(BulletPatternId, Enemy *enemy);

