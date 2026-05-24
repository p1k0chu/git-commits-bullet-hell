// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "my_math.hpp"

#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <math.h>

Vec2d::Vec2d() : Vec2d(0.0, 0.0) {}

Vec2d::Vec2d(double x, double y) : x(x), y(y) {}

Vec2d Vec2d::from_angle(double radian) {
    return Vec2d(SDL_cos(radian), SDL_sin(radian));
}

double Vec2d::magnitude() const {
    return SDL_sqrt(SDL_pow(x, 2) + SDL_pow(y, 2));
}

double Vec2d::magnitude_sqr() const {
    return SDL_pow(x, 2) + SDL_pow(y, 2);
}

Vec2d Vec2d::get_normal() const {
    const double direction = SDL_atan2(y, x) + SDL_PI_D / 2;
    return Vec2d(SDL_cos(direction), SDL_sin(direction));
}

Vec2d Vec2d::project_on(const Vec2d &other) const {
    const double ab = dot_product(other);
    const double bb = other.dot_product(other);
    const double i = ab / bb;
    return Vec2d(other.x * i, other.y * i);
}

Vec2d Vec2d::rotate(double radian) const {
    const double c = SDL_cos(radian);
    const double s = SDL_sin(radian);
    return Vec2d(c * x - y * s, x * s + y * c);
}

double Vec2d::angle_rad() const {
    return SDL_atan2(y, x);
}

double Vec2d::angle_deg() const {
    return angle_rad() * 180.0 / SDL_PI_D;
}

double Vec2d::angle2(const Vec2d &other) const {
    return SDL_acos(dot_product(other) / magnitude() / other.magnitude());
}

double Vec2d::scalar_projection(const Vec2d &onto) const {
    const double angle = angle2(onto);
    // if angle is nan, one of the vectors is a zero vector,
    // thus scalar projection is always 0
    if (SDL_isnan(angle))
        return 0;

    assert(angle <= SDL_PI_D);

    const Vec2d proj = project_on(onto);
    const double sign = (angle <= SDL_PI_D / 2) ? 1 : -1;
    return sign * proj.magnitude();
}

double Vec2d::dot_product(const Vec2d &other) const {
    return x * other.x + y * other.y;
}

Vec2d Vec2d::look_at(Vec2d other) const {
    double radian = SDL_atan2(other.y - y, other.x - x);
    return Vec2d(SDL_cos(radian), SDL_sin(radian));
}

Vec2d Vec2d::operator+(const Vec2d &other) const {
    return Vec2d(x + other.x, y + other.y);
}

Vec2d Vec2d::operator-(const Vec2d &other) const {
    return Vec2d(x - other.x, y - other.y);
}

Vec2d Vec2d::operator*(double other) const {
    return Vec2d(x * other, y * other);
}

bool Vec2d::operator==(const Vec2d &other) const {
    return x == other.x && y == other.y;
}

Vec2d Vec2d::right(double d) const {
    return Vec2d(x + d, y);
}

Vec2d Vec2d::left(double d) const {
    return Vec2d(x - d, y);
}

Vec2d Vec2d::above(double d) const {
    return Vec2d(x, y - d);
}

Vec2d Vec2d::below(double d) const {
    return Vec2d(x, y + d);
}

int polygons_collide(const Vec2d *const normals,
                     const size_t normals_len,
                     const Vec2d *const dots_poly1,
                     const size_t dots_poly1_len,
                     const Vec2d *const dots_poly2,
                     const size_t dots_poly2_len) {
    size_t i, j;

    for (i = 0; i < normals_len; ++i) {
        const Vec2d *normal = normals + i;

        double poly1_min = INFINITY;
        double poly1_max = -INFINITY;

        for (j = 0; j < dots_poly1_len; ++j) {
            const Vec2d *point = dots_poly1 + j;
            const double m = point->scalar_projection(*normal);
            if (m > poly1_max)
                poly1_max = m;
            if (m < poly1_min)
                poly1_min = m;
        }

        double poly2_min = INFINITY;
        double poly2_max = -INFINITY;

        for (j = 0; j < dots_poly2_len; ++j) {
            const Vec2d *point = dots_poly2 + j;
            const double m = point->scalar_projection(*normal);
            if (m > poly2_max)
                poly2_max = m;
            if (m < poly2_min)
                poly2_min = m;
        }

        if ((poly1_min < poly2_min && poly1_max < poly2_min) ||
            (poly1_min > poly2_max && poly1_max > poly2_max)) {
            return 0;
        }
    }
    return 1;
}

double fix_angle(double degrees) {
    while (degrees <= -180.0) degrees += 360.0;
    while (degrees > 180.0) degrees -= 360.0;
    return degrees;
}

double rad_to_deg(double d) {
    return d * 180.0 / SDL_PI_D;
}

double deg_to_rad(double d) {
    return d / 180.0 * SDL_PI_D;
}
