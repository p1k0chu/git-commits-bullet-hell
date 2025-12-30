#include "pattern.h"

#include "main.h"
#include "utils.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

