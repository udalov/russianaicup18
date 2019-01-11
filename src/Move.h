#pragma once

#include "Vec.h"

struct Move {
    Vec targetVelocity;
    double jumpSpeed;
    bool useNitro;

    Move() :
        Move(Vec(), double(), false) {}

    Move(const Vec& targetVelocity, double jumpSpeed, bool useNitro) :
        targetVelocity(targetVelocity), jumpSpeed(jumpSpeed), useNitro(useNitro) {}

    std::string toString() const;
};
