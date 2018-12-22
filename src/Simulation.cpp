#include "Simulation.h"

#include <algorithm>

using namespace model;
using namespace std;

struct Dan {
    double distance;
    Vec normal;

    Dan() :
        distance(1e100), normal() {}

    Dan(double distance, const Vec& normal) :
        distance(distance), normal(normal) {}
};

void danToPlane(const Vec& point, const Vec& pointOnPlane, const Vec& planeNormal, Dan& result) {
    auto distance = (point - pointOnPlane).dot(planeNormal);
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = planeNormal;
    }
}

void danToArenaQuarter(const Vec& point, Dan& result) {
    danToPlane(point, Vec(0, 0, 0), Vec(0, 1, 0), result);
}

void danToArena(Vec point, Dan& result) {
    auto negateX = point.x < 0;
    auto negateZ = point.z < 0;
    if (negateX) point.x = -point.x;
    if (negateZ) point.z = -point.z;
    danToArenaQuarter(point, result);
    if (negateX) result.normal.x = -result.normal.x;
    if (negateZ) result.normal.z = -result.normal.z;
}

void updateRobot(RobotState& robot, double deltaTime, const Rules& rules, const Move& move) {
    if (robot.touch) {
        auto targetVelocity = Vec(move.targetVelocity);
        targetVelocity.clamp(rules.ROBOT_MAX_GROUND_SPEED);
        targetVelocity -= robot.touchNormal * robot.touchNormal.dot(targetVelocity);
        auto targetVelocityChange = targetVelocity - robot.velocity;
        auto len = targetVelocityChange.len();
        if (len > 0) {
            auto acceleration = rules.ROBOT_ACCELERATION * max(0.0, robot.touchNormal.y);
            auto tmp = targetVelocityChange.normalize() * acceleration * deltaTime;
            tmp.clamp(len);
            robot.velocity += tmp;
        }
    }

    robot.velocity.clamp(rules.ROBOT_MAX_GROUND_SPEED);
    robot.position += robot.velocity * deltaTime;
    double tmp = rules.GRAVITY * deltaTime;
    robot.position.y -= tmp * deltaTime / 2;
    robot.velocity.y -= tmp;

    robot.radius = rules.ROBOT_MIN_RADIUS + (rules.ROBOT_MAX_RADIUS - rules.ROBOT_MIN_RADIUS) *
        move.jumpSpeed / rules.ROBOT_MAX_JUMP_SPEED;
    robot.radiusChangeSpeed = move.jumpSpeed;
}

bool collideRobotArena(RobotState& robot, double deltaTime, const Rules& rules, Vec& result) {
    Dan dan;
    danToArena(robot.position, dan);
    auto penetration = robot.radius - dan.distance;
    if (penetration > 0) {
        robot.position += dan.normal * penetration;
        auto speed = robot.velocity.dot(dan.normal) - robot.radiusChangeSpeed;
        if (speed < 0) {
            robot.velocity -= dan.normal * ((1 + rules.ROBOT_ARENA_E) * speed);
            return true;
        }
    }
    return false;
}
