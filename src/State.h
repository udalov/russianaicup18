#pragma once

#include "Vec.h"
#include <string>
#include <vector>

struct RobotState {
    int id;
    Vec position;
    Vec velocity;
    double radius;
    double radiusChangeSpeed;
    Vec touchNormal;
    bool touch;

    RobotState(int id, const Vec& position, const Vec& velocity, double radius, const Vec& touchNormal, bool touch) :
        id(id), position(position), velocity(velocity), radius(radius), radiusChangeSpeed(), touchNormal(touchNormal), touch(touch) {}

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
    std::vector<RobotState> robots;
    BallState ball;
    int goal; // 1 if allies scored, -1 if enemies scored, 0 otherwise

    State(std::vector<RobotState>&& robots, BallState&& ball) :
        robots(robots), ball(ball), goal(0) {}

    const RobotState& findRobotById(int id) const;
};
