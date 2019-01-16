#include "MyStrategy.h"

#include "Const.h"
#include "Move.h"
#include "QuickStartGuy.h"
#include "Simulation.h"
#include "Solution.h"
#include "State.h"
#include "Team.h"
#include "Vec.h"
#include "Vis.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>
#include <unordered_map>

using namespace model;
using namespace std;

#define VIS 0
#define LOG 0
#define MEASURE_SIMULATION_SPEED 0

#ifdef LOCAL
#define TICK_INFO 1000
#else
#define TICK_INFO 2000
#endif

MyStrategy::MyStrategy() = default;

namespace {
    unique_ptr<Vis> vis = nullptr;
    unordered_map<int, Move> answers;
    unordered_map<int, Scenario *> scenario;
}

RobotState createRobot(const Robot& robot) {
    return RobotState(
        robot.id, Vec(robot.x, robot.y, robot.z), Vec(robot.velocity_x, robot.velocity_y, robot.velocity_z), robot.radius,
        robot.nitro_amount, Vec(robot.touch_normal_x, robot.touch_normal_y, robot.touch_normal_z), robot.touch
    );
}

vector<RobotState> createRobots(const vector<Robot>& robots) {
    vector<RobotState> result;
    result.reserve(robots.size());
    transform(robots.begin(), robots.end(), back_inserter(result), createRobot);
    sort(result.begin(), result.end(), [](auto& r1, auto& r2) { return r1.id < r2.id; });
    return result;
}

array<NitroPackState, 4> createNitroPacks(const vector<NitroPack>& nitroPacks) {
    if (nitroPacks.empty()) return {};

    vector<NitroPackState> result(4);
    for (auto& pack : nitroPacks) {
        result[getClosestNitroPackIndex(pack.x, pack.z)] = NitroPackState(pack.alive, pack.respawn_ticks);
    }
    return { result[0], result[1], result[2], result[3] };
}

BallState createBall(const Ball& ball) {
    return BallState(Vec(ball.x, ball.y, ball.z), Vec(ball.velocity_x, ball.velocity_y, ball.velocity_z));
}

pair<int, int> getScore(const Game& game) {
    int allies = 0;
    int enemies = 0;
    for (auto& player : game.players) {
        if (player.id == getAllies().playerId) allies = player.score;
        else enemies = player.score;
    }
    return make_pair(allies, enemies);
}

pair<int, int> previousScore = { 0, 0 };

void solve(State&& state, const pair<int, int>& score, int currentTick, bool debug) {
    if (currentTick == 0) {
        auto enemyStrategy = new QuickStartGuy<false>();
        auto solution = new Solution(getAllies(), *enemyStrategy);
        for (auto& robot : state.robots) {
            if (isAlly(robot.id)) {
                scenario[robot.id] = solution;
            } else {
                scenario[robot.id] = enemyStrategy;
            }
        }
    }

    if (score != previousScore) {
        previousScore = score;
        for (auto& robot : state.robots) {
            scenario[robot.id]->goalScored(currentTick);
        }
    }

    auto keyBase = currentTick << 4;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) {
            answers[keyBase + robot.id] = scenario[robot.id]->getMove(state, robot, currentTick, 0);
        }
    }

    if (debug) {
        auto getMove = [currentTick, keyBase](const State& state, const RobotState& robot, size_t delta) {
            if (delta == 0 && isAlly(robot.id)) {
                return answers[keyBase + robot.id];
            }

            return scenario[robot.id]->getMove(state, robot, currentTick, delta);
        };

        if (VIS && LOG) {
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

        simulate(state, TRACK_LEN, MICROTICKS, VIS ? vis.get() : nullptr, getMove);
    }
}

Move solve(const Game& game, int myId, bool debug) {
    auto tick = game.current_tick;
    auto key = (tick << 4) + myId;
    auto answer = answers.find(key);
    if (answer != answers.end()) return answer->second;

    solve(State(createRobots(game.robots), createNitroPacks(game.nitro_packs), createBall(game.ball)), getScore(game), tick, debug);

    return answers[key];
}

namespace {
    vector<double> times;
    double allTime;
}

void addTime(double time) {
    if (time >= 0.002) {
        times.push_back(time);
    }
}

void printTimes() {
    if (times.empty()) {
        cout << endl;
        return;
    }
    auto minmax = minmax_element(times.begin(), times.end());
    auto sum = accumulate(times.begin(), times.end(), 0.0);
    auto average = sum / times.size();
    cout << times.size() << " [" << *minmax.first << " " << average << " " << *minmax.second << "] " << sum << endl;

    allTime += sum;
    times.clear();
}

void printTickInfo(const Game& game, const Rules& rules) {
    auto currentTick = game.current_tick;
    auto isLastTick = currentTick == rules.max_tick_count - 1;
    if (currentTick % TICK_INFO == 0 || isLastTick) {
        auto score = getScore(game);
        cout << "tick " << currentTick << " " << score.first << ":" << score.second << " ";
        printTimes();
    }
    if (isLastTick) {
        cout << allTime << endl;
    }
}

void MyStrategy::act(const Robot& me, const Rules& rules, const Game& game, Action& action) {
    // printConst(rules); terminate();

    auto startTime = chrono::high_resolution_clock::now();

    if (game.current_tick == 0) {
        initializeTeams(game);
        cout.precision(3);
        cout << fixed;
    }

    if (me.id == getCaptain()) {
#ifdef LOCAL
        if (debug) {
#endif
            printTickInfo(game, rules);
#ifdef LOCAL
        }
#endif
    }

    if (debug) {
        if (VIS && vis == nullptr) {
            vis = make_unique<Vis>();
            vis->drawArena();
        }
    }

    auto ans = solve(game, me.id, debug);

    action.target_velocity_x = ans.targetVelocity.x;
    action.target_velocity_y = ans.targetVelocity.y;
    action.target_velocity_z = ans.targetVelocity.z;
    action.jump_speed = ans.jumpSpeed;
    action.use_nitro = ans.useNitro;

    if (debug) {
        if (me.id == getCaptain() && VIS) {
            vis->drawGame(me, game);
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    addTime(chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count());
}
