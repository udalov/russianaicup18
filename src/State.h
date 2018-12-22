#pragma once

#include "Vec.h"

struct RobotState {
    int id;
    Vec position;
    Vec velocity;
    double radius;
    double radiusChangeSpeed;
    Vec touchNormal;
    bool touch;

    RobotState(int id, const Vec& position, const Vec& velocity, double radius) :
        id(id), position(position), velocity(velocity), radius(radius), radiusChangeSpeed(/* TODO?! */), touchNormal(), touch() {}
};
