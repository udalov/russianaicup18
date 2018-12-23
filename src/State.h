#pragma once

#include "Move.h"
#include "Vec.h"
#include <functional>
#include <string>

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

    std::string toString() const;
};

struct BallState {
    Vec position;
    Vec velocity;

    BallState(const Vec& position, const Vec& velocity) :
        position(position), velocity(velocity) {}

    std::string toString() const;
};

struct State {
    RobotState me;
    BallState ball;
    std::function<Move(int /* id */, int /* tick (relative to current tick) */)> moves;

    State(const RobotState& me, const BallState& ball, std::function<Move(int, int)>&& moves) :
        me(me), ball(ball), moves(moves) {}
};
