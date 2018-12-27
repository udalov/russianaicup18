#include "MyStrategy.h"

#include "Const.h"
#include "Move.h"
#include "QuickStartGuy.h"
#include "Simulation.h"
#include "Solution.h"
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
#define MEASURE_SIMULATION_SPEED 1

MyStrategy::MyStrategy() { }

namespace {
    unique_ptr<Vis> vis = nullptr;
    unordered_map<int, Move> answers;
    unordered_map<int, Scenario *> scenario;
    QuickStartGuy quickStart;
    Solution solution;
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
    if (currentTick == 0) {
        for (auto& robot : state.robots) {
            if (isAlly(robot.id)) {
                scenario[robot.id] = &solution;
            } else {
                scenario[robot.id] = &quickStart;
            }
        }
    }

    auto keyBase = currentTick << 4;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) {
            answers[keyBase + robot.id] = scenario[robot.id]->getMove(state, robot, currentTick, 0);
        }
    }

    auto getMove = [currentTick, keyBase](const State& state, const RobotState& robot, int delta) {
        if (delta == 0 && isAlly(robot.id)) {
            return answers[keyBase + robot.id];
        }

        return scenario[robot.id]->getMove(state, robot, currentTick, delta);
    };

    if (VIS) {
        vis->addLog(state.findRobotById(getCaptain()).toString());
        vis->addLog(state.ball.toString());
        vis->addLog(answers[keyBase + getCaptain()].toString());
        vis->addLog("");

        for (auto& robot : state.robots) {
            if (robot.id != getCaptain()) {
                vis->addLog(robot.toString());
                vis->addLog(string() + "predicted " + getMove(state, robot, 0).toString());
                vis->addLog("");
            }
        }
    }

    if (MEASURE_SIMULATION_SPEED) {
        auto ticks = 50000;
        auto microticks = 100;
        auto beginTime = clock();
        simulate(state, ticks, microticks, nullptr, getMove);
        auto endTime = clock();
        cout << ticks << " ticks x " << microticks << " microticks in " << (endTime - beginTime + 0.0) / CLOCKS_PER_SEC << "s" << endl;

        auto checksum = state.ball.position.x + state.ball.position.y + state.ball.position.z;
        for (auto& robot : state.robots) checksum += robot.position.x + robot.position.y + robot.position.z;
        cout << "checksum " << checksum << endl;

        throw string() + "OK";
    }

    simulate(state, 50, MICROTICKS_PER_TICK, VIS ? vis.get() : nullptr, getMove);
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
}
