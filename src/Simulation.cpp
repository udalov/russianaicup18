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

namespace {
    const Rules *rules = nullptr;
    Vis *vis = nullptr;
}

const Rules& getRules() {
    return *rules;
}

void initializeSimulation(const Rules& initRules, Vis& initVis) {
    rules = &initRules;
    vis = &initVis;
}

void danToPlane(const Vec& point, const Vec& pointOnPlane, const Vec& planeNormal, Dan& result) {
    auto distance = (point - pointOnPlane).dot(planeNormal);
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = planeNormal;
    }
}

void danToSphereInner(const Vec& point, const Vec& center, double radius, Dan& result) {
    auto distance = radius - (point - center).len();
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = (center - point).normalize();
    }
}

void danToSphereOuter(const Vec& point, const Vec& center, double radius, Dan& result) {
    auto distance = (point - center).len() - radius;
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = (point - center).normalize();
    }
}

void danToArenaQuarter(const Vec& point, Dan& result) {
    auto& arena = rules->arena;

    // Ground
    danToPlane(point, Vec(0, 0, 0), Vec(0, 1, 0), result);

    // Ceiling
    danToPlane(point, Vec(0, arena.height, 0), Vec(0, -1, 0), result);

    // Side x
    danToPlane(point, Vec(arena.width / 2, 0, 0), Vec(-1, 0, 0), result);

    // Side z (goal)
    danToPlane(point, Vec(0, 0, (arena.depth / 2) + arena.goal_depth), Vec(0, 0, -1), result);

    // Side z
    auto vx = point.x - (arena.goal_width / 2 - arena.goal_top_radius);
    auto vy = point.y - (arena.goal_height - arena.goal_top_radius);
    if (point.x >= arena.goal_width / 2 + arena.goal_side_radius ||
        point.y >= arena.goal_height + arena.goal_side_radius ||
        (vx > 0 && vy > 0 && sqr(vx) + sqr(vy) >= sqr(arena.goal_top_radius + arena.goal_side_radius))) {
        danToPlane(point, Vec(0, 0, arena.depth / 2), Vec(0, 0, -1), result);
    }

    // Side x & ceiling (goal)
    if (point.z >= arena.depth / 2 + arena.goal_side_radius) {
        // x
        danToPlane(point, Vec(arena.goal_width / 2, 0, 0), Vec(-1, 0, 0), result);
        // y
        danToPlane(point, Vec(0, arena.goal_height, 0), Vec(0, -1, 0), result);
    }

    // Goal back corners
    // TODO

    // Corner
    if (point.x > arena.width / 2 - arena.corner_radius && point.z > arena.depth / 2 - arena.corner_radius) {
        auto center = Vec(arena.width / 2 - arena.corner_radius, point.y, arena.depth / 2 - arena.corner_radius);
        danToSphereInner(point, center, arena.corner_radius, result);
    }

    // Goal outer corner
    if (point.z < arena.depth / 2 + arena.goal_side_radius) {
        // Side x
        if (point.x < arena.goal_width / 2 + arena.goal_side_radius) {
            auto center = Vec(arena.goal_width / 2 + arena.goal_side_radius, point.y, arena.depth / 2 + arena.goal_side_radius);
            danToSphereOuter(point, center, arena.goal_side_radius, result);
        }

        // Ceiling
        if (point.y < arena.goal_height + arena.goal_side_radius) {
            auto center = Vec(point.x, arena.goal_height + arena.goal_side_radius, arena.depth / 2 + arena.goal_side_radius);
            danToSphereOuter(point, center, arena.goal_side_radius, result);
        }

        // Top corner
        auto o = Vec(arena.goal_width / 2 - arena.goal_top_radius, arena.goal_height - arena.goal_top_radius, 0);
        auto v = Vec(point.x, point.y, 0) - o;
        if (v.x > 0 and v.y > 0) {
            o += v.normalize() * (arena.goal_top_radius + arena.goal_side_radius);
            auto center = Vec(o.x, o.y, arena.depth / 2 + arena.goal_side_radius);
            danToSphereOuter(point, center, arena.goal_side_radius, result);
        }
    }

    // Goal inside top corners
    // TODO    

    // Bottom corners
    if (point.y < arena.bottom_radius) {
        // Side x
        if (point.x > arena.width / 2 - arena.bottom_radius) {
            auto center = Vec(arena.width / 2 - arena.bottom_radius, arena.bottom_radius, point.z);
            danToSphereInner(point, center, arena.bottom_radius, result);
        }

        // Side z
        if (point.z > arena.depth / 2 - arena.bottom_radius && point.x >= arena.goal_width / 2 + arena.goal_side_radius) {
            auto center = Vec(point.x, arena.bottom_radius, arena.depth / 2 - arena.bottom_radius);
            danToSphereInner(point, center, arena.bottom_radius, result);
        }

        // Side z (goal)
        if (point.z > arena.depth / 2 + arena.goal_depth - arena.bottom_radius) {
            auto center = Vec(point.x, arena.bottom_radius, arena.depth / 2 + arena.goal_depth - arena.bottom_radius);
            danToSphereInner(point, center, arena.bottom_radius, result);
        }

        // Goal outer corner
        auto o = Vec(
            arena.goal_width / 2 + arena.goal_side_radius,
            arena.depth / 2 + arena.goal_side_radius,
            0
        );
        auto v = Vec(point.x, point.z, 0) - o;
        if (v.x < 0 && v.y < 0 && v.sqrLen() < sqr(arena.goal_side_radius + arena.bottom_radius)) {
            o += v.normalize() * (arena.goal_side_radius + arena.bottom_radius);
            danToSphereInner(point, Vec(o.x, arena.bottom_radius, o.y), arena.bottom_radius, result);
        }

        // Side x (goal)
        if (point.z >= arena.depth / 2 + arena.goal_side_radius && point.x > arena.goal_width / 2 - arena.bottom_radius) {
            auto center = Vec(arena.goal_width / 2 - arena.bottom_radius, arena.bottom_radius, point.z);
            danToSphereInner(point, center, arena.bottom_radius, result);
        }

        // Corner
        if (point.x > arena.width / 2 - arena.corner_radius && point.z > arena.depth / 2 - arena.corner_radius) {
            auto o = Vec(
                arena.width / 2 - arena.corner_radius,
                arena.depth / 2 - arena.corner_radius,
                0
            );
            auto n = Vec(point.x, point.z, 0) - o;
            auto sqrDist = n.sqrLen();
            if (sqrDist > sqr(arena.corner_radius - arena.bottom_radius)) {
                o += n * ((arena.corner_radius - arena.bottom_radius) / sqrt(sqrDist));
                danToSphereInner(point, Vec(o.x, arena.bottom_radius, o.y), arena.bottom_radius, result);
            }
        }
    }

    // Ceiling corners
    // TODO
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

template<typename Unit>
void moveUnit(Unit& unit, double deltaTime) {
    unit.velocity.clamp(rules->MAX_ENTITY_SPEED);
    unit.position += unit.velocity * deltaTime;
    double tmp = rules->GRAVITY * deltaTime;
    unit.position.y -= tmp * deltaTime / 2;
    unit.velocity.y -= tmp;
}

void updateRobot(RobotState& robot, double deltaTime, const Move& move) {
    if (robot.touch) {
        auto targetVelocity = Vec(move.targetVelocity);
        targetVelocity.clamp(rules->ROBOT_MAX_GROUND_SPEED);
        targetVelocity -= robot.touchNormal * robot.touchNormal.dot(targetVelocity);
        auto targetVelocityChange = targetVelocity - robot.velocity;
        auto len = targetVelocityChange.len();
        if (len > 0) {
            auto acceleration = rules->ROBOT_ACCELERATION * max(0.0, robot.touchNormal.y);
            auto tmp = targetVelocityChange.normalize() * acceleration * deltaTime;
            tmp.clamp(len);
            robot.velocity += tmp;
        }
    }

    moveUnit(robot, deltaTime);

    robot.radius = rules->ROBOT_MIN_RADIUS + (rules->ROBOT_MAX_RADIUS - rules->ROBOT_MIN_RADIUS) *
        move.jumpSpeed / rules->ROBOT_MAX_JUMP_SPEED;
    robot.radiusChangeSpeed = move.jumpSpeed;
}

template<typename Unit> double getUnitRadius(const Unit& unit);
template<> double getUnitRadius(const RobotState& unit) { return unit.radius; }
template<> double getUnitRadius(const BallState& unit) { return rules->BALL_RADIUS; }

template<typename Unit> double getUnitRadiusChangeSpeed(const Unit& unit);
template<> double getUnitRadiusChangeSpeed(const RobotState& unit) { return unit.radiusChangeSpeed; }
template<> double getUnitRadiusChangeSpeed(const BallState& unit) { return 0.0; }

template<typename Unit> double getArenaE();
template<> double getArenaE<RobotState>() { return rules->ROBOT_ARENA_E; }
template<> double getArenaE<BallState>() { return rules->BALL_ARENA_E; }

template<typename Unit>
bool collideWithArena(Unit& unit, double deltaTime, Vec& result) {
    Dan dan;
    danToArena(unit.position, dan);
    auto penetration = getUnitRadius(unit) - dan.distance;
    if (penetration > 0) {
        unit.position += dan.normal * penetration;
        auto speed = unit.velocity.dot(dan.normal) - getUnitRadiusChangeSpeed(unit);
        if (speed < 0) {
            unit.velocity -= dan.normal * ((1 + getArenaE<Unit>()) * speed);
            result = dan.normal;
            return true;
        }
    }
    return false;
}

void update(State& state, int tick, double deltaTime) {
    auto& robot = state.me;
    auto move = state.moves(state.me.id, tick);

    updateRobot(robot, deltaTime, move);

    moveUnit(state.ball, deltaTime);

    Vec collisionNormal;
    if (collideWithArena(robot, deltaTime, collisionNormal)) {
        robot.touch = true;
        robot.touchNormal = collisionNormal;
    } else {
        robot.touch = false;
    }

    collideWithArena(state.ball, deltaTime, collisionNormal);
}

void simulate(State& state, int ticks) {
    const int microticks = rules->MICROTICKS_PER_TICK;

    for (int tick = 1; tick <= ticks; tick++) {
        auto deltaTime = 1.0 / rules->TICKS_PER_SECOND / microticks;
        for (int _ = 1; _ <= microticks; _++) {
            update(state, tick, deltaTime);
        }

        auto myPos = state.me.position;
        auto radius = state.me.radius;
        auto ballPos = state.ball.position;
        vis->addAction([=](Vis& v) {
            v.drawSphere(myPos.x, myPos.y, myPos.z, radius, Color::ME.alpha(0.25 + ((ticks - tick) / 3.0 / ticks)));
            v.drawSphere(ballPos.x, ballPos.y, ballPos.z, rules->BALL_RADIUS, Color::BALL.alpha(0.25 + ((ticks - tick) / 3.0 / ticks)));
        });
        vis->addLog(state.me.toString());
        vis->addLog(state.ball.toString());
    }
}
