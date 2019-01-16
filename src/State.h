#pragma once

#include "Vec.h"
#include <array>
#include <string>
#include <vector>

struct RobotState {
    int id;
    Vec position;
    Vec velocity;
    double radius;
    double radiusChangeSpeed;
    double nitro;
    Vec touchNormal;
    bool touch;

    RobotState(int id, const Vec& position, const Vec& velocity, double radius, double nitro, const Vec& touchNormal, bool touch) :
        id(id), position(position), velocity(velocity), radius(radius), radiusChangeSpeed(), nitro(nitro), touchNormal(touchNormal), touch(touch) {}

    std::string toString() const;
};

struct BallState {
    Vec position;
    Vec velocity;

    BallState(const Vec& position, const Vec& velocity) :
        position(position), velocity(velocity) {}

    std::string toString() const;
};

struct NitroPackState {
    int respawnTicks;

    NitroPackState() :
        respawnTicks(1000000) {}

    NitroPackState(bool alive, int respawnTicks) :
        respawnTicks(alive ? 0 : respawnTicks) {}

    bool isAlive() const { return respawnTicks == 0; }
};

struct State {
    std::vector<RobotState> robots;
    std::array<NitroPackState, 4> nitroPacks;
    BallState ball;
    int goal; // 1 if allies scored, -1 if enemies scored, 0 otherwise

    State(std::vector<RobotState>&& robots, std::array<NitroPackState, 4>&& nitroPacks, BallState&& ball) :
        robots(robots), nitroPacks(nitroPacks), ball(ball), goal(0) {}

    const RobotState& findRobotById(int id) const;
};

size_t getClosestNitroPackIndex(double x, double z);
