#include "MyStrategy.h"

#include "Move.h"
#include "Simulation.h"
#include "State.h"
#include "Vec.h"
#include "Vis.h"

#include <cmath>
#include <iostream>
#include <memory>

using namespace model;
using namespace std;

MyStrategy::MyStrategy() { }

unique_ptr<Vis> vis = nullptr;

void simulateRobotSameMove(RobotState& robot, const Rules& rules, const Move& move) {
    const int ticks = 20;
    const int microticks = rules.MICROTICKS_PER_TICK;

    Vec collisionNormal;
    for (int tick = 1; tick <= ticks; tick++) {
        double deltaTime = 1.0 / rules.TICKS_PER_SECOND / microticks;
        for (int _ = 1; _ <= microticks; _++) {
            updateRobot(robot, deltaTime, rules, move);
            if (collideRobotArena(robot, deltaTime, rules, collisionNormal)) {
                robot.touch = true;
                robot.touchNormal = collisionNormal;
            } else {
                robot.touch = false;
            }
        }
        Vec pos = robot.position;
        double radius = robot.radius;
        vis->addAction([=](Vis& v) {
            v.drawSphere(pos.x, pos.y, pos.z, radius, Color::ME.alpha(0.25 + ((ticks - tick) / 3.0 / ticks)));
        });
    }
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    if (me.id != 1) return;

    auto tick = game.current_tick;
    if (tick % 100 == 0) cout << "tick " << tick << endl;

    if (tick == 0) {
        vis = make_unique<Vis>(rules.arena);
        vis->drawArena();
    }

    auto pos = Vec(me.x, me.y, me.z);
    auto velocity = Vec(me.velocity_x, me.velocity_y, me.velocity_z);

    /*
    action.target_velocity_x = (game.ball.x - me.x) * 1000.0;
    action.target_velocity_z = (game.ball.z - me.z) * 1000.0;
    */
    Move ans;
    if (abs(pos.x) > rules.arena.width / 2 - rules.arena.bottom_radius - 2*me.radius) {
        ans = Move(Vec(0, 0, 0), rules.ROBOT_MAX_JUMP_SPEED);
    } else {
        ans = Move(Vec(-rules.ROBOT_MAX_GROUND_SPEED, 0, 0), 0);
    }

    auto robot = RobotState(me.id, pos, velocity, me.radius);
    simulateRobotSameMove(robot, rules, ans);

    /*
    velocity.clamp(rules.ROBOT_MAX_GROUND_SPEED);
    for (int t = 1; t < 10; t++) {
        pos += velocity * (1.0 / rules.TICKS_PER_SECOND);
        vis->addAction([=](Vis& v) {
            v.drawSphere(pos.x, pos.y, pos.z, me.radius, Color::ME.alpha(0.25 + ((10.0-t)/30.0)));
        });
    }
    */

    action.target_velocity_x = ans.targetVelocity.x;
    action.target_velocity_y = ans.targetVelocity.y;
    action.target_velocity_z = ans.targetVelocity.z;
    action.jump_speed = ans.jumpSpeed;

    if (tick < 3000) {
        vis->drawGame(me, game);
    } else {
        terminate();
    }
}
