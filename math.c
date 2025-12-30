#include "math.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float Vec2f_magnitude(const Vec2f *this) {
    return sqrtf(powf(this->x, 2) + powf(this->y, 2));
}

Vec2f Vec2f_get_normal(const Vec2f *this) {
    const float direction = atan2f(this->y, this->x) + M_PI_2;
    return (Vec2f){cosf(direction), sinf(direction)};
}

Vec2f Vec2f_project_on(const Vec2f *this, const Vec2f *other) {
    const float ab = dot_product(this, other);
    const float bb = dot_product(other, other);
    const float i  = ab / bb;
    return (Vec2f){other->x * i, other->y * i};
}

Vec2f Vec2f_rotate(const Vec2f *this, float rotation_radian) {
    const float c = cosf(rotation_radian);
    const float s = sinf(rotation_radian);
    return (Vec2f){
        c * this->x - this->y * s,
        this->x * s + this->y * c,
    };
}

Vec2f Vec2f_add(const Vec2f *a, const Vec2f *b) {
    return (Vec2f){a->x + b->x, a->y + b->y};
}

float dot_product(const Vec2f *left, const Vec2f *right) {
    return left->x * right->x + left->y * right->y;
}

bool collide(Player *player, Enemy *enemy) {
    const float enemy_rot = enemy->rotation * M_PI / 180.0f;

    const float enemy_rot_2 = enemy_rot + M_PI_2;

    const Vec2f all_normals[] = {// player's normals
                                 {1, 0},
                                 {0, 1},

                                 // enemy's normals
                                 {cosf(enemy_rot), sinf(enemy_rot)},
                                 {cosf(enemy_rot_2), sinf(enemy_rot_2)}};

    const float p_l = player->x - player->w / 2;
    const float p_r = player->x + player->w / 2;
    const float p_t = player->y - player->h / 2;
    const float p_b = player->y + player->h / 2;

    const Vec2f all_player_points[] = {
        {p_l, p_t}, // top left
        {p_l, p_b}, // bottom left
        {p_r, p_t}, // top right
        {p_r, p_b}  // bottom right
    };

    const Vec2f e_center = {enemy->rect.x + enemy->rect.w / 2, enemy->rect.y + enemy->rect.h / 2};

    Vec2f all_enemy_points[4] = {};

    // top left
    Vec2f tmp           = {-enemy->rect.w / 2, -enemy->rect.h / 2};
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[0] = Vec2f_add(&tmp, &e_center);

    // top right
    tmp.x               = enemy->rect.w / 2;
    tmp.y               = -enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[1] = Vec2f_add(&tmp, &e_center);

    // bottom left
    tmp.x               = -enemy->rect.w / 2;
    tmp.y               = enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[2] = Vec2f_add(&tmp, &e_center);

    // bottom right
    tmp.x               = enemy->rect.w / 2;
    tmp.y               = enemy->rect.h / 2;
    tmp                 = Vec2f_rotate(&tmp, enemy_rot);
    all_enemy_points[3] = Vec2f_add(&tmp, &e_center);

    for (size_t i = 0; i < sizeof(all_normals) / sizeof(all_normals[0]); ++i) {
        const Vec2f *normal = all_normals + i;

        float player_min = INFINITY;
        float player_max = -INFINITY;

        for (size_t j = 0; j < sizeof(all_player_points) / sizeof(all_player_points[0]); ++j) {
            const Vec2f *point = all_player_points + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > player_max) player_max = m;
            if (m < player_min) player_min = m;
        }

        float enemy_min = INFINITY;
        float enemy_max = -INFINITY;

        for (size_t j = 0; j < sizeof(all_enemy_points) / sizeof(all_enemy_points[0]); ++j) {
            const Vec2f *point = all_enemy_points + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > enemy_max) enemy_max = m;
            if (m < enemy_min) enemy_min = m;
        }

        if ((player_min < enemy_min && player_max < enemy_min) ||
            (player_min > enemy_max && player_max > enemy_max)) {
            return false;
        }
    }
    return true;
}

