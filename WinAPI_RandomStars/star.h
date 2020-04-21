#pragma once

#include <cmath>
#include <vector>
#include <Windows.h>

const double PI = std::acos(-1.0);
#define RAD(deg) deg * PI / 180.0
#define DEG(rad) rad * 180.0 / PI

static struct dPoint
{
    double x, y;
};

class Star
{
private:
    dPoint center = { };
    dPoint velocity = { };
    double angle;
    double rotate;
    double ldist;
    double ratio;

public:
    Star(double x, double y, double vX, double vY, double angle, double rotate, double ldistance, double ratio = 0.36);
    //Star(const Star&);

    void negVelocityX() {
        velocity.x = -velocity.x;
    }

    void negVelocityY() {
        velocity.y = -velocity.y;
    }

    void move(const double time) {
        center.x += velocity.x * time;
        center.y += velocity.y * time;
        angle += rotate * time;
        //angle -= (angle / 72) * 72;
        angle = fmod(angle, RAD(72.0));
    }

    double getCenterX() const {
        return center.x;
    }

    double getCenterY() const {
        return center.y;
    }

    POINT* getPoints() const;
};

Star::Star(double x, double y, double vX, double vY, double angle, double rotate, double ldistance, double ratio) {
    this->center.x = x;
    this->center.y = y;
    this->velocity.x = vX;
    this->velocity.y = vY;
    this->angle = RAD(fmod(angle, 72.0));
    this->rotate = RAD(rotate);
    this->ldist = ldistance;
    this->ratio = ratio;
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