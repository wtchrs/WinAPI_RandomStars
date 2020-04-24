#pragma once

#include <cmath>
#include <vector>
#include <Windows.h>

const DOUBLE PI = std::acos(-1.0);
#define RAD(deg) deg * PI / 180.0
#define DEG(rad) rad * 180.0 / PI

struct dPoint
{
    DOUBLE x, y;
};

class Star
{
private:
    dPoint center   = { };
    dPoint velocity = { };
    DOUBLE angle    = 0.0;
    DOUBLE rotate   = 0.0;
    DOUBLE ldist    = 0.0;
    DOUBLE ratio    = 0.0;

public:
    Star(DOUBLE x, DOUBLE y, DOUBLE vX, DOUBLE vY, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio = 0.36);

    ~Star() { }

    void negVelocityX() {
        velocity.x = -velocity.x;
    }

    void negVelocityY() {
        velocity.y = -velocity.y;
    }

    void move(const DOUBLE time) {
        center.x += velocity.x * time;
        center.y += velocity.y * time;
        angle    += rotate * time;
        angle     = fmod(angle, RAD(72.0));
    }

    DOUBLE getCenterX() const {
        return center.x;
    }

    DOUBLE getCenterY() const {
        return center.y;
    }

    POINT* getPoints() const;
};

Star::Star(DOUBLE x, DOUBLE y, DOUBLE vx, DOUBLE vy, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio) {
    this->center   = { x, y };
    this->velocity = { vx, vy };
    this->angle    = RAD(fmod(angle, 72.0));
    this->rotate   = RAD(rotate);
    this->ldist    = ldistance;
    this->ratio    = ratio;
}

POINT* Star::getPoints() const {
    POINT points[10] = { };

    for (int i = 0; i < 10; i+=2) {
        points[i] = POINT {
            static_cast<LONG>(ldist * std::cos(angle + RAD(72.0) * i) + center.x),
            static_cast<LONG>(ldist * std::sin(angle + RAD(72.0) * i) + center.y)
        };

        points[i+1] = POINT {
            static_cast<LONG>(ldist * ratio * std::cos(angle + RAD(72.0) * i + RAD(36.0)) + center.x),
            static_cast<LONG>(ldist * ratio * std::sin(angle + RAD(72.0) * i + RAD(36.0)) + center.y)
        };
    }

    return points;
}