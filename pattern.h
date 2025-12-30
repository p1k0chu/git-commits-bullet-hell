#pragma once

typedef enum BulletPatternId {
    Dummy,
    Dummy2,
} BulletPatternId;

#define BULLET_PATTERN_ID_LEN 2

bool should_spawn_enemies(BulletPatternId, unsigned long time_ms);
bool should_start_next_pattern(BulletPatternId, unsigned long time_since_pattern_start_ms);
void spawn_enemies(BulletPatternId);

