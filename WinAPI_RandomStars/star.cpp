#include "star.h"

Star::Star(DOUBLE x, DOUBLE y, DOUBLE vx, DOUBLE vy, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio){
    this->center = { x, y };
    this->velocity = { vx, vy };
    this->angle = RAD(fmod(angle, 72.0));
    this->rotate = RAD(rotate);
    this->ldist = ldistance;
    this->ratio = ratio;
}

Star::Star(COLORREF color, DOUBLE x, DOUBLE y, DOUBLE vx, DOUBLE vy, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio){
    this->color = color;
    this->center = { x, y };
    this->velocity = { vx, vy };
    this->angle = RAD(fmod(angle, 72.0));
    this->rotate = RAD(rotate);
    this->ldist = ldistance;
    this->ratio = ratio;
}

POINT *Star::get_points() const{
    POINT points[10] = { };

    for (int i = 0; i < 10; i += 2){
        points[i] = POINT {
            static_cast<LONG>(ldist * std::cos(angle + RAD(72.0) * i) + center.x),
            static_cast<LONG>(ldist * std::sin(angle + RAD(72.0) * i) + center.y)
        };

        points[i + 1] = POINT {
            static_cast<LONG>(ldist * ratio * std::cos(angle + RAD(72.0) * i + RAD(36.0)) + center.x),
            static_cast<LONG>(ldist * ratio * std::sin(angle + RAD(72.0) * i + RAD(36.0)) + center.y)
        };
    }

    return points;
}