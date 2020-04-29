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
    COLORREF color  = RGB(0, 0, 0);
    dPoint center   = { };
    dPoint velocity = { };
    DOUBLE angle    = 0.0;
    DOUBLE rotate   = 0.0;
    DOUBLE ldist    = 0.0;
    DOUBLE ratio    = 0.0;

public:
    Star(DOUBLE x, DOUBLE y, DOUBLE vX, DOUBLE vY, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio = 0.36);
    Star(COLORREF color, DOUBLE x, DOUBLE y, DOUBLE vX, DOUBLE vY, DOUBLE angle, DOUBLE rotate, DOUBLE ldistance, DOUBLE ratio = 0.36);

    ~Star() { }

    void neg_velocity_x() {
        velocity.x = -velocity.x;
    }

    void neg_velocity_y() {
        velocity.y = -velocity.y;
    }

    void move(const DOUBLE time) {
        center.x += velocity.x * time;
        center.y += velocity.y * time;
        angle    += rotate * time;
        angle     = fmod(angle, RAD(72.0));
    }

    DOUBLE get_center_x() const {
        return center.x;
    }

    DOUBLE get_center_y() const {
        return center.y;
    }

    COLORREF get_color() const{
        return color;
    }

    POINT* get_points() const;
};