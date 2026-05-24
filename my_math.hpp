// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#pragma once

#include <stddef.h>

struct Vec2d {
    Vec2d();
    Vec2d(double x, double y);

    static Vec2d from_angle(double radian);

    double magnitude() const;
    double magnitude_sqr() const;
    Vec2d get_normal() const;
    Vec2d project_on(const Vec2d& other) const;
    Vec2d rotate(double radian) const;
    double angle_rad() const;
    double angle_deg() const;
    double angle2(const Vec2d& other) const;
    double scalar_projection(const Vec2d& onto) const;
    double dot_product(const Vec2d& other) const;

    /// @brief returns a unit vector pointing at the dot specified by `other`
    Vec2d look_at(Vec2d other) const;

    Vec2d operator+(const Vec2d& other) const;
    Vec2d operator-(const Vec2d& other) const;
    Vec2d operator*(double) const;
    bool operator==(const Vec2d& other) const;

    Vec2d right(double) const;
    Vec2d left(double) const;
    Vec2d above(double) const;
    Vec2d below(double) const;

    double x, y;
};

int polygons_collide(const Vec2d* normals,
                     size_t normals_len,
                     const Vec2d* dots_poly1,
                     size_t dots_poly1_len,
                     const Vec2d* dots_poly2,
                     size_t dots_poly2_len);

double fix_angle(double degrees);
double rad_to_deg(double);
double deg_to_rad(double);

