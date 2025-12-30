#include "player.h"

void Player_get_points(const Player *this, Vec2d dst[4]) {
    const double p_l = this->x - this->w / 2;
    const double p_r = this->x + this->w / 2;
    const double p_t = this->y - this->h / 2;
    const double p_b = this->y + this->h / 2;

    // top left
    dst[0].x = p_l;
    dst[0].y = p_t;

    // bottom left
    dst[1].x = p_l;
    dst[1].y = p_b;

    // top right
    dst[2].x = p_r;
    dst[2].y = p_t;

    // bottom right
    dst[3].x = p_r;
    dst[3].y = p_b;
}

