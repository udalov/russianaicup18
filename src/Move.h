#pragma once

#include "Vec.h"

struct Move {
    Vec targetVelocity;
    double jumpSpeed;

    Move() :
        Move(Vec(), double()) {}

    Move(const Vec& targetVelocity, double jumpSpeed) :
        targetVelocity(targetVelocity), jumpSpeed(jumpSpeed) {}
};
