#pragma once

#include <stddef.h>

typedef struct Vec2f {
    float x, y;
} Vec2f;

float Vec2f_magnitude(const Vec2f *this);
Vec2f Vec2f_get_normal(const Vec2f *this);
Vec2f Vec2f_project_on(const Vec2f *this, const Vec2f *other);
Vec2f Vec2f_rotate(const Vec2f *this, float rotation_radian);
Vec2f Vec2f_add(const Vec2f *a, const Vec2f *b);

float dot_product(const Vec2f *left, const Vec2f *right);

bool polygons_collide(const Vec2f *normals,
                      size_t       normals_len,
                      const Vec2f *dots_poly1,
                      size_t       dots_poly1_len,
                      const Vec2f *dots_poly2,
                      size_t       dots_poly2_len);

