#include "MyStrategy.h"

#include "Move.h"
#include "QuickStartGuy.h"
#include "Simulation.h"
#include "State.h"
#include "Static.h"
#include "Vec.h"
#include "Vis.h"

#include <cmath>
#include <iostream>
#include <memory>

using namespace model;
using namespace std;

MyStrategy::MyStrategy() { }

unique_ptr<Vis> vis = nullptr;

RobotState createRobot(const Robot& robot) {
    return RobotState(
        robot.id, Vec(robot.x, robot.y, robot.z), Vec(robot.velocity_x, robot.velocity_y, robot.velocity_z), robot.radius,
        Vec(robot.touch_normal_x, robot.touch_normal_y, robot.touch_normal_z), robot.touch
    );
}

vector<RobotState> createRobots(const vector<Robot>& robots) {
    vector<RobotState> result;
    result.reserve(robots.size());
    transform(robots.begin(), robots.end(), back_inserter(result), createRobot);
    sort(result.begin(), result.end(), [](auto& r1, auto& r2) { return r1.id < r2.id; });
    return result;
}

BallState createBall(const Ball& ball) {
    return BallState(Vec(ball.x, ball.y, ball.z), Vec(ball.velocity_x, ball.velocity_y, ball.velocity_z));
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    auto currentTick = game.current_tick;

    if (isCaptain(me.id)) {
        if (currentTick % 100 == 0) cout << "tick " << currentTick << endl;

        if (currentTick == 0) {
            initializeStatic(rules, game);
            vis = make_unique<Vis>(rules.arena);
            vis->drawArena();
        }
    }

    Move *myMove = nullptr;

    auto state = State(
        createRobots(game.robots),
        createBall(game.ball),
        me.id,
        [currentTick](const State& state, const RobotState& robot, int tick) {
            // return robot.id == me.id ? *myMove : quickStartMove(state, robot);
            return quickStartMove(state, robot, currentTick, tick);
        }
    );

    /*
    Move ans;
    if (abs(me.x) > getArena().width / 2 - getArena().bottom_radius - 2*me.radius) {
        ans = Move(Vec(0, 0, 0), getRules().ROBOT_MAX_JUMP_SPEED);
    } else {
        ans = Move(Vec(-getRules().ROBOT_MAX_GROUND_SPEED, 0, 0), 0);
    }
    */

    Move ans = state.moves(state, state.findRobotById(me.id), 0);
    myMove = &ans;

    if (isCaptain(me.id)) {
        vis->addLog(createRobot(me).toString());
        vis->addLog(createBall(game.ball).toString());
        vis->addLog(ans.toString());
        vis->addLog("");

        for (auto& robot : state.robots) {
            if (robot.id == me.id) continue;

            vis->addLog(string() + (isAlly(robot.id) ? "ally " : "enemy ") + robot.toString());
            vis->addLog(string() + "predicted " + state.moves(state, robot, 0).toString());
            vis->addLog("");
        }

        simulate(state, 50, vis.get());
    }

    action.target_velocity_x = ans.targetVelocity.x;
    action.target_velocity_y = ans.targetVelocity.y;
    action.target_velocity_z = ans.targetVelocity.z;
    action.jump_speed = ans.jumpSpeed;

    if (isCaptain(me.id)) {
        vis->drawGame(me, game);
    }

    if (currentTick == 5000) {
        terminate();
    }
}
