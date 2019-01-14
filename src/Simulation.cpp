#include "Simulation.h"

#include "Const.h"
#include "Team.h"
#include <array>
#include <algorithm>

using namespace model;
using namespace std;

struct Dan {
    double distance;
    Vec normal;

    Dan() :
        distance(1e100), normal() {}
};

void danToPlane(const Vec& point, Vec&& pointOnPlane, Vec&& planeNormal, Dan& result) {
    pointOnPlane -= point;
    auto distance = -pointOnPlane.dot(planeNormal);
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = planeNormal;
    }
}

void danToSphereInner(const Vec& point, Vec&& center, double radius, Dan& result) {
    center -= point;
    auto len = center.len();
    auto distance = radius - len;
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = center * (1 / len);
    }
}

void danToSphereOuter(const Vec& point, Vec&& center, double radius, Dan& result) {
    center -= point;
    auto len = center.len();
    auto distance = len - radius;
    if (distance < result.distance) {
        result.distance = distance;
        result.normal = center * (-1 / len);
    }
}

constexpr double EPS = 1e-3;

void danToArenaQuarter(const Vec& point, Dan& result) {
    // Ground
    danToPlane(point, Vec(0, 0, 0), Vec(0, 1, 0), result);

    if (point.y > ARENA_H - BALL_RADIUS - EPS) {
        // Ceiling
        danToPlane(point, Vec(0, ARENA_H, 0), Vec(0, -1, 0), result);
    }

    if (point.x > ARENA_W / 2 - ARENA_BR - BALL_RADIUS - EPS) {
        // Side x
        danToPlane(point, Vec(ARENA_W / 2, 0, 0), Vec(-1, 0, 0), result);
    }

    if (point.z > ARENA_D / 2 + ARENA_GD - ARENA_GTR - BALL_RADIUS - EPS) {
        // Side z (goal)
        danToPlane(point, Vec(0, 0, ARENA_D / 2 + ARENA_GD), Vec(0, 0, -1), result);
    }

    if (point.z > ARENA_D / 2 - BALL_RADIUS - EPS) {
        // Side z
        auto vx = point.x - (ARENA_GW / 2 - ARENA_GTR);
        auto vy = point.y - (ARENA_GH - ARENA_GTR);
        if (point.x >= ARENA_GW / 2 + ARENA_GSR ||
            point.y >= ARENA_GH + ARENA_GSR ||
            (vx > 0 && vy > 0 && sqr(vx) + sqr(vy) >= sqr(ARENA_GTR + ARENA_GSR))) {
            danToPlane(point, Vec(0, 0, ARENA_D / 2), Vec(0, 0, -1), result);
        }
    }

    // Side z & ceiling (goal)
    if (point.z >= ARENA_D / 2 + ARENA_GSR) {
        // x
        danToPlane(point, Vec(ARENA_GW / 2, 0, 0), Vec(-1, 0, 0), result);
        // y
        danToPlane(point, Vec(0, ARENA_GH, 0), Vec(0, -1, 0), result);
    }

    // Goal back corners
    // TODO

    // Corner
    if (point.x > ARENA_W / 2 - ARENA_CR && point.z > ARENA_D / 2 - ARENA_CR) {
        danToSphereInner(point, Vec(ARENA_W / 2 - ARENA_CR, point.y, ARENA_D / 2 - ARENA_CR), ARENA_CR, result);
    }

    // Goal outer corner
    if (point.z < ARENA_D / 2 + ARENA_GSR && point.z > ARENA_D / 2 - BALL_RADIUS - EPS) {
        // Side x
        if (point.x < ARENA_GW / 2 + ARENA_GSR && point.x > ARENA_GW / 2 - BALL_RADIUS - EPS) {
            danToSphereOuter(point, Vec(ARENA_GW / 2 + ARENA_GSR, point.y, ARENA_D / 2 + ARENA_GSR), ARENA_GSR, result);
        }

        // Ceiling
        if (point.y < ARENA_GH + ARENA_GSR && point.y > ARENA_GH - BALL_RADIUS - EPS) {
            danToSphereOuter(point, Vec(point.x, ARENA_GH + ARENA_GSR, ARENA_D / 2 + ARENA_GSR), ARENA_GSR, result);
        }

        // Top corner
        auto o = Vec(ARENA_GW / 2 - ARENA_GTR, ARENA_GH - ARENA_GTR, 0);
        auto v = Vec(point.x, point.y, 0) - o;
        if (v.x > 0 && v.y > 0) {
            o += v.normalize() * (ARENA_GTR + ARENA_GSR);
            danToSphereOuter(point, Vec(o.x, o.y, ARENA_D / 2 + ARENA_GSR), ARENA_GSR, result);
        }
    }

    // Goal inside top corners
    // TODO

    // Bottom corners
    if (point.y < ARENA_BR) {
        // Side x
        if (point.x > ARENA_W / 2 - ARENA_BR) {
            danToSphereInner(point, Vec(ARENA_W / 2 - ARENA_BR, ARENA_BR, point.z), ARENA_BR, result);
        }

        // Side z
        if (point.z > ARENA_D / 2 - ARENA_BR && point.x >= ARENA_GW / 2 + ARENA_GSR) {
            danToSphereInner(point, Vec(point.x, ARENA_BR, ARENA_D / 2 - ARENA_BR), ARENA_BR, result);
        }

        // Side z (goal)
        if (point.z > ARENA_D / 2 + ARENA_GD - ARENA_BR) {
            danToSphereInner(point, Vec(point.x, ARENA_BR, ARENA_D / 2 + ARENA_GD - ARENA_BR), ARENA_BR, result);
        }

        if (point.x > ARENA_GW / 2 - ARENA_GSR - BALL_RADIUS - EPS && point.z > ARENA_D / 2 - ARENA_GSR - BALL_RADIUS - EPS) {
            // Goal outer corner
            auto o = Vec(ARENA_GW / 2 + ARENA_GSR, ARENA_D / 2 + ARENA_GSR, 0);
            auto v = Vec(point.x, point.z, 0) - o;
            if (v.x < 0 && v.y < 0 && v.sqrLen() < sqr(ARENA_GSR + ARENA_BR)) {
                o += v.normalize() * (ARENA_GSR + ARENA_BR);
                danToSphereInner(point, Vec(o.x, ARENA_BR, o.y), ARENA_BR, result);
            }
        }

        // Side x (goal)
        if (point.z >= ARENA_D / 2 + ARENA_GSR && point.x > ARENA_GW / 2 - ARENA_BR) {
            danToSphereInner(point, Vec(ARENA_GW / 2 - ARENA_BR, ARENA_BR, point.z), ARENA_BR, result);
        }

        // Corner
        if (point.x > ARENA_W / 2 - ARENA_CR && point.z > ARENA_D / 2 - ARENA_CR) {
            auto o = Vec(ARENA_W / 2 - ARENA_CR, ARENA_D / 2 - ARENA_CR, 0);
            auto n = Vec(point.x, point.z, 0) - o;
            auto sqrDist = n.sqrLen();
            if (sqrDist > sqr(ARENA_CR - ARENA_BR)) {
                o += n * ((ARENA_CR - ARENA_BR) / sqrt(sqrDist));
                danToSphereInner(point, Vec(o.x, ARENA_BR, o.y), ARENA_BR, result);
            }
        }
    }

    // Ceiling corners
    if (point.y > ARENA_H - ARENA_TR) {
        // Side x
        if (point.x > ARENA_W / 2 - ARENA_TR) {
            danToSphereInner(point, Vec(ARENA_W / 2 - ARENA_TR, ARENA_H - ARENA_TR, point.z), ARENA_TR, result);
        }

        // Side z
        if (point.z > ARENA_D / 2 - ARENA_TR) {
            danToSphereInner(point, Vec(point.x, ARENA_H - ARENA_TR, ARENA_D / 2 - ARENA_TR), ARENA_TR, result);
        }

        // Corner
        if (point.x > ARENA_W / 2 - ARENA_CR && point.z > ARENA_D / 2 - ARENA_CR) {
            auto o = Vec(ARENA_W / 2 - ARENA_CR, 0, ARENA_D / 2 - ARENA_CR);
            auto dv = Vec(point.x, 0, point.z) - o;
            if (dv.sqrLen() > sqr(ARENA_CR - ARENA_TR)) {
                auto n = dv.normalize();
                o += n * (ARENA_CR - ARENA_TR);
                danToSphereInner(point, Vec(o.x, ARENA_H - ARENA_TR, o.z), ARENA_TR, result);
            }
        }
    }
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
    unit.velocity.clamp(MAX_ENTITY_SPEED);
    unit.position += unit.velocity * deltaTime;
    auto tmp = GRAVITY * deltaTime;
    unit.position.y -= tmp * deltaTime / 2;
    unit.velocity.y -= tmp;
}

void updateRobot(RobotState& robot, double deltaTime, const Move& move) {
    if (robot.touch) {
        auto tv = move.targetVelocity;
        // tv.clamp(ROBOT_MAX_GROUND_SPEED);
        tv -= robot.touchNormal * robot.touchNormal.dot(tv);
        tv -= robot.velocity;
        auto len = tv.len();
        if (len > 0) {
            auto acceleration = ROBOT_ACCELERATION * max(0.0, robot.touchNormal.y);
            auto coeff = acceleration * deltaTime / len;
            if (coeff < 1) {
                tv *= coeff;
            }
            robot.velocity += tv;
        }
    }

    if (move.useNitro && robot.nitro > 0) {
        auto tv = move.targetVelocity - robot.velocity;
        auto len = tv.len();
        auto limit = robot.nitro * NITRO_POINT_VELOCITY_CHANGE;
        if (len > limit) {
            tv *= limit / len;
            len = limit;
        }
        if (len > 0) {
            auto coeff = ROBOT_NITRO_ACCELERATION * deltaTime / len;
            if (coeff < 1) {
                tv *= coeff;
            }
            robot.velocity += tv;
            constexpr auto nitroDepletionSpeed = ROBOT_NITRO_ACCELERATION / NITRO_POINT_VELOCITY_CHANGE;
            robot.nitro -= deltaTime * nitroDepletionSpeed;
        }
    }

    moveUnit(robot, deltaTime);

    robot.radius = ROBOT_MIN_RADIUS + (ROBOT_MAX_RADIUS - ROBOT_MIN_RADIUS) *
        move.jumpSpeed / ROBOT_MAX_JUMP_SPEED;
    robot.radiusChangeSpeed = move.jumpSpeed;
}

template<typename Unit> constexpr double getUnitRadius(const Unit& unit);
template<> constexpr double getUnitRadius(const RobotState& unit) { return unit.radius; }
template<> constexpr double getUnitRadius(const BallState& unit) { return BALL_RADIUS; }

template<typename Unit> constexpr double getUnitRadiusChangeSpeed(const Unit& unit);
template<> constexpr double getUnitRadiusChangeSpeed(const RobotState& unit) { return unit.radiusChangeSpeed; }
template<> constexpr double getUnitRadiusChangeSpeed(const BallState& unit) { return 0.0; }

template<typename Unit> constexpr double getArenaE();
template<> constexpr double getArenaE<RobotState>() { return ROBOT_ARENA_E; }
template<> constexpr double getArenaE<BallState>() { return BALL_ARENA_E; }

template<typename Unit> constexpr double getUnitMass();
template<> constexpr double getUnitMass<RobotState>() { return ROBOT_MASS; }
template<> constexpr double getUnitMass<BallState>() { return BALL_MASS; }

template<typename Unit>
void collideEntities(RobotState& a, Unit& b) {
    auto d = b.position - a.position;
    auto needed = a.radius + getUnitRadius(b);
    if (d.x > needed || d.x < -needed || d.y > needed || d.y < -needed || d.z > needed || d.z < -needed) return;

    auto penetration = needed - d.len();
    if (penetration <= 0) return;

    constexpr auto amass = ROBOT_MASS;
    constexpr auto bmass = getUnitMass<Unit>();
    constexpr auto ka = (1 / amass) / ((1 / amass) + (1 / bmass));
    constexpr auto kb = (1 / bmass) / ((1 / amass) + (1 / bmass));
    auto normal = d.normalize();
    a.position -= normal * penetration * ka;
    b.position += normal * penetration * kb;
    auto deltaSpeed = (b.velocity - a.velocity).dot(normal) - getUnitRadiusChangeSpeed(b) - a.radiusChangeSpeed;
    if (deltaSpeed < 0) {
        constexpr auto random = (MIN_HIT_E + MAX_HIT_E) / 2;
        // constexpr auto random = MAX_HIT_E;
        auto impulse = normal * ((1 + random) * deltaSpeed);
        a.velocity += impulse * ka;
        b.velocity -= impulse * kb;
    }
}

template<typename Unit>
bool collideWithArena(Unit& unit, Vec& result) {
    Dan dan;
    danToArena(unit.position, dan);
    auto penetration = getUnitRadius(unit) - dan.distance;
    if (penetration > 0) {
        auto& normal = dan.normal;
        unit.position += normal * penetration;
        auto speed = unit.velocity.dot(normal) - getUnitRadiusChangeSpeed(unit);
        if (speed < 0) {
            unit.velocity -= normal * ((1 + getArenaE<Unit>()) * speed);
            result = normal;
            return true;
        }
    }
    return false;
}

namespace {
    array<Move, 10> moves;
}

void update(State& state, double deltaTime) {
    auto& robots = state.robots;

    for (size_t i = 0; i < robots.size(); i++) {
        updateRobot(robots[i], deltaTime, moves[i]);
    }

    moveUnit(state.ball, deltaTime);

    for (size_t i = 0; i < robots.size(); i++) {
        for (size_t j = 0; j < i; j++) {
            collideEntities(robots[i], robots[j]);
        }
    }

    Vec collisionNormal;
    for (auto& robot : robots) {
        collideEntities(robot, state.ball);
        if (collideWithArena(robot, collisionNormal)) {
            robot.touch = true;
            robot.touchNormal = collisionNormal;
        } else {
            robot.touch = false;
        }
    }

    collideWithArena(state.ball, collisionNormal);

    auto ballZ = state.ball.position.z;
    if (ballZ > ARENA_D / 2 + BALL_RADIUS) {
        state.goal = 1;
    } else if (ballZ < -ARENA_D / 2 - BALL_RADIUS) {
        state.goal = -1;
    }

    // TODO: update nitro packs
}

void simulate(State& state, int ticks, int microticks, Vis *vis, const function<Move(const State&, const RobotState&, size_t)>& getMove) {
    auto deltaTime = 1.0 / TICKS_PER_SECOND / microticks;

    for (int tick = 0; tick < ticks; tick++) {
        for (size_t i = 0; i < state.robots.size(); i++) {
            moves[i] = getMove(state, state.robots[i], tick);
            moves[i].targetVelocity.clamp(ROBOT_MAX_GROUND_SPEED);
        }

        /*
        if (vis != nullptr) {
            auto myIndex = 0;
            while (state.robots[myIndex].id != getCaptain()) myIndex++;
            vis->addLog(moves[myIndex].toString());
        }
        */

        for (int _ = microticks; _ > 0; _--) {
            update(state, deltaTime);
        }

        if (vis != nullptr) {
            for (auto& robot : state.robots) {
                auto pos = robot.position;
                auto radius = robot.radius;
                auto colorBase = robot.id == getCaptain() ? Color::ME : isAlly(robot.id) ? Color::ALLY : Color::ENEMY;
                auto color = colorBase.alpha(0.25 + ((ticks - tick) / 3.0 / ticks));
                vis->addAction([pos, radius, color](Vis& v) {
                    v.drawSphere(pos.x, pos.y, pos.z, radius, color);
                });
                // if (tick < 2) vis->addLog(string() + "+" + to_string(tick) + " " + robot.toString());
            }
            auto ballPos = state.ball.position;
            auto ballColor = Color::BALL.alpha(0.25 + ((ticks - tick) / 3.0 / ticks));
            vis->addAction([ballPos, ballColor](Vis& v) {
                v.drawSphere(ballPos.x, ballPos.y, ballPos.z, BALL_RADIUS, ballColor);
            });
            // if (tick < 2) vis->addLog(string() + "+" + to_string(tick) + " " + state.ball.toString());
            // vis->addLog(state.findRobotById(getCaptain()).toString());
            // vis->addLog(state.ball.toString());
        }
    }
}
