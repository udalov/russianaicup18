#include "MyStrategy.h"

#include "Const.h"
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
#include <unordered_map>

using namespace model;
using namespace std;

#define VIS 1

MyStrategy::MyStrategy() { }

namespace {
    unique_ptr<Vis> vis = nullptr;
    unordered_map<int, Move> answers;
}

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

void solve(State&& state, int solverId, int currentTick) {
    auto keyBase = currentTick << 4;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) {
            answers[keyBase + robot.id] = quickStartMove(state, robot, currentTick, 0);
        }
    }

    auto& captain = state.findRobotById(getCaptain());
    if (VIS) {
        vis->addLog(captain.toString());
        vis->addLog(state.ball.toString());
        vis->addLog(answers[keyBase + captain.id].toString());
        vis->addLog("");
    }

    auto getMove = [currentTick, keyBase](const State& state, const RobotState& robot, int delta) {
        if (delta == 0) {
            auto it = answers.find(keyBase + robot.id);
            if (it != answers.end()) return it->second;
        }
        return quickStartMove(state, robot, currentTick, delta);
    };

    for (auto& robot : state.robots) {
        if (robot.id == captain.id) continue;

        if (VIS) {
            vis->addLog(string() + (isAlly(robot.id) ? "ally " : "enemy ") + robot.toString());
            vis->addLog(string() + "predicted " + getMove(state, robot, 0).toString());
            vis->addLog("");
        }
    }

    simulate(state, 50, MICROTICKS_PER_TICK, VIS ? vis.get() : nullptr, getMove);

    /*
    auto ticks = 50000;
    auto microticks = 100;
    auto beginTime = clock();
    simulate(state, ticks, microticks, VIS ? vis.get() : nullptr, getMove);
    auto endTime = clock();
    cout << ticks << " ticks x " << microticks << " microticks in " << (endTime - beginTime + 0.0) / CLOCKS_PER_SEC << "s" << endl;

    auto checksum = state.ball.position.x + state.ball.position.y + state.ball.position.z;
    for (auto& robot : state.robots) checksum += robot.position.x + robot.position.y + robot.position.z;
    cout << "checksum " << checksum << endl;

    terminate();
    */
}

Move solve(const Game& game, int myId) {
    auto tick = game.current_tick;
    auto key = (tick << 4) + myId;
    auto answer = answers.find(key);
    if (answer != answers.end()) return answer->second;

    solve(State(createRobots(game.robots), createBall(game.ball)), myId, tick);

    return answers[key];
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    // printConst(rules); terminate();

    auto currentTick = game.current_tick;

    if (currentTick == 0) {
        initializeStatic(game);
    }

    if (me.id == getCaptain()) {
        if (currentTick % 100 == 0) cout << "tick " << currentTick << endl;
    }

    if (VIS && vis == nullptr) {
        vis = make_unique<Vis>();
        vis->drawArena();
    }

    auto ans = solve(game, me.id);

    action.target_velocity_x = ans.targetVelocity.x;
    action.target_velocity_y = ans.targetVelocity.y;
    action.target_velocity_z = ans.targetVelocity.z;
    action.jump_speed = ans.jumpSpeed;

    if (me.id == getCaptain() && VIS) {
        vis->drawGame(me, game);
    }

    if (currentTick == 5000) {
        terminate();
    }
}
