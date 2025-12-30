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

