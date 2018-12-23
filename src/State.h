#pragma once

#include "Move.h"
#include "Vec.h"
#include <functional>
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
    int myId;
    std::function<Move(const State&, const RobotState&, int /* tick (relative to current tick) */)> moves;

    State(std::vector<RobotState>&& robots, BallState&& ball, int myId, std::function<Move(const State&, const RobotState&, int)>&& moves) :
        robots(robots), ball(ball), myId(myId), moves(moves) {}

    const RobotState& findRobotById(int id) const;
};
