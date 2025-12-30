#include "math.h"

#include <math.h>

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

bool polygons_collide(const Vec2f *const normals,
                      const size_t       normals_len,
                      const Vec2f *const dots_poly1,
                      const size_t       dots_poly1_len,
                      const Vec2f *const dots_poly2,
                      const size_t       dots_poly2_len) {
    for (size_t i = 0; i < normals_len; ++i) {
        const Vec2f *normal = normals + i;

        float poly1_min = INFINITY;
        float poly1_max = -INFINITY;

        for (size_t j = 0; j < dots_poly1_len; ++j) {
            const Vec2f *point = dots_poly1 + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > poly1_max) poly1_max = m;
            if (m < poly1_min) poly1_min = m;
        }

        float poly2_min = INFINITY;
        float poly2_max = -INFINITY;

        for (size_t j = 0; j < dots_poly2_len; ++j) {
            const Vec2f *point = dots_poly2 + j;
            const Vec2f  proj  = Vec2f_project_on(point, normal);
            const float  m     = Vec2f_magnitude(&proj);
            if (m > poly2_max) poly2_max = m;
            if (m < poly2_min) poly2_min = m;
        }

        if ((poly1_min < poly2_min && poly1_max < poly2_min) ||
            (poly1_min > poly2_max && poly1_max > poly2_max)) {
            return false;
        }
    }
    return true;
}

