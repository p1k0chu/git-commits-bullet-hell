#pragma once

#include <stddef.h>

typedef struct Vec2d {
    double x, y;
} Vec2d;

double Vec2d_magnitude(const Vec2d *this);
Vec2d  Vec2d_get_normal(const Vec2d *this);
Vec2d  Vec2d_project_on(const Vec2d *this, const Vec2d *other);
Vec2d  Vec2d_rotate(const Vec2d *this, double rotation_radian);
Vec2d  Vec2d_add(const Vec2d *a, const Vec2d *b);

double dot_product(const Vec2d *left, const Vec2d *right);

bool polygons_collide(const Vec2d *normals,
                      size_t       normals_len,
                      const Vec2d *dots_poly1,
                      size_t       dots_poly1_len,
                      const Vec2d *dots_poly2,
                      size_t       dots_poly2_len);

