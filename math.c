#include "math.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

double Vec2d_magnitude(const Vec2d *this) {
    return sqrt(pow(this->x, 2) + pow(this->y, 2));
}

Vec2d Vec2d_get_normal(const Vec2d *this) {
    const double direction = atan2(this->y, this->x) + M_PI_2;
    return (Vec2d){cos(direction), sin(direction)};
}

Vec2d Vec2d_project_on(const Vec2d *this, const Vec2d *other) {
    const double ab = dot_product(this, other);
    const double bb = dot_product(other, other);
    const double i  = ab / bb;
    return (Vec2d){other->x * i, other->y * i};
}

Vec2d Vec2d_rotate(const Vec2d *this, double rotation_radian) {
    const double c = cos(rotation_radian);
    const double s = sin(rotation_radian);
    return (Vec2d){
        c * this->x - this->y * s,
        this->x * s + this->y * c,
    };
}

Vec2d Vec2d_add(const Vec2d *a, const Vec2d *b) {
    return (Vec2d){a->x + b->x, a->y + b->y};
}

double dot_product(const Vec2d *left, const Vec2d *right) {
    return left->x * right->x + left->y * right->y;
}

double Vec2d_angle2(const Vec2d *a, const Vec2d *b) {
    return acos(dot_product(a, b) / Vec2d_magnitude(a) / Vec2d_magnitude(b));
}

double Vec2d_scalar_projection(const Vec2d *this, const Vec2d *onto) {
    const double angle = Vec2d_angle2(this, onto);
    // if angle is nan, one of the vectors is a zero vector,
    // thus scalar projection is always 0
    if (isnan(angle)) return 0;

    assert(angle <= M_PI);

    const Vec2d  proj = Vec2d_project_on(this, onto);
    const double sign = (angle <= M_PI_2) ? 1 : -1;
    return sign * Vec2d_magnitude(&proj);
}

bool polygons_collide(const Vec2d *const normals,
                      const size_t       normals_len,
                      const Vec2d *const dots_poly1,
                      const size_t       dots_poly1_len,
                      const Vec2d *const dots_poly2,
                      const size_t       dots_poly2_len) {
    for (size_t i = 0; i < normals_len; ++i) {
        const Vec2d *normal = normals + i;

        double poly1_min = INFINITY;
        double poly1_max = -INFINITY;

        for (size_t j = 0; j < dots_poly1_len; ++j) {
            const Vec2d *point = dots_poly1 + j;
            const double m     = Vec2d_scalar_projection(point, normal);
            if (m > poly1_max) poly1_max = m;
            if (m < poly1_min) poly1_min = m;
        }

        double poly2_min = INFINITY;
        double poly2_max = -INFINITY;

        for (size_t j = 0; j < dots_poly2_len; ++j) {
            const Vec2d *point = dots_poly2 + j;
            const double m     = Vec2d_scalar_projection(point, normal);
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

