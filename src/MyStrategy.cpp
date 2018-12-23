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

void simulateRobotSameMove(RobotState& robot, BallState& ball, const Rules& rules, const Move& move) {
    vis->addLog(robot.toString());
    vis->addLog(ball.toString());
    vis->addLog(move.toString());
    vis->addLog("");

    auto state = State(
        robot,
        ball,
        [&](int id, int tick) {
            return move;
        }
    );

    simulate(state, 20);
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    if (me.id != 1) return;

    auto tick = game.current_tick;
    if (tick % 100 == 0) cout << "tick " << tick << endl;

    if (tick == 0) {
        vis = make_unique<Vis>(rules.arena);
        vis->drawArena();
        initializeSimulation(rules, *vis);
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
    auto ball = BallState(
        Vec(game.ball.x, game.ball.y, game.ball.z),
        Vec(game.ball.velocity_x, game.ball.velocity_y, game.ball.velocity_z)
    );
    simulateRobotSameMove(robot, ball, rules, ans);

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

    if (tick < 500) {
        vis->drawGame(me, game);
    } else {
        terminate();
    }
}
