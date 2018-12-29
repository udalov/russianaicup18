#include "Solution.h"

#include "Const.h"
#include "Simulation.h"
#include <iostream>
#include <random>
#include <vector>

// TODO: remove
#include "QuickStartGuy.h"

using namespace std;

namespace {
    mt19937 rng(42);

    vector<Move> orders(1000 * ORDER_LEN);
    size_t nextOrderIndex = 0;
}

struct Order {
    size_t index;

    Order(size_t index) : index(index) {}

    Move& get(size_t robotIndex, int delta) {
        return orders[index + robotIndex * TRACK_LEN + delta];
    }

    Move& operator()(size_t robotIndex, int delta) { return get(robotIndex, delta); }
    const Move& operator()(size_t robotIndex, int delta) const { return const_cast<Order *>(this)->get(robotIndex, delta); }
};

void resetOrderPool() {
    nextOrderIndex = 0;
}

Order getNextOrder() {
    auto result = Order(nextOrderIndex);
    nextOrderIndex += ORDER_LEN;
    return result;
}

double score(State state, const Order& order, const Team& team) {
    QuickStartGuy q;

    simulate(state, TRACK_LEN, MICROTICKS, nullptr, [&q, &order, &team](const State& state, const RobotState& robot, int delta) {
        auto index = team.getIndex(robot.id);
        // TODO (!): somehow pass strategy of the other team
        if (index == Team::NONE) {
            // return Move();
            return q.getMove(state, robot, 0, delta);
        }

        return order(index, delta);
    });

    double ans = 0;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) ans -= robot.position.distance(state.ball.position);
    }
    ans += 10 * state.ball.position.z;
    ans -= 100 * state.ball.velocity.distance(Vec(0, ARENA_H/2, ARENA_D) - state.ball.position);

    return ans;
}

void compute(const State& state, const Team& team, array<Move, ORDER_LEN>& bestOrder, int tick) {
    resetOrderPool();

    auto result = getNextOrder();
    for (size_t j = 0; j < team.size; j++) {
        size_t shift = TRACK_LEN * j;
        for (size_t i = 1; i < TRACK_LEN; i++) {
            result(j, i - 1) = bestOrder[shift + i];
        }
        result(j, TRACK_LEN - 1) = result(j, TRACK_LEN - 2);
    }

    uniform_int_distribution<size_t> randomRobotIndex(0, team.size - 1);
    uniform_int_distribution<size_t> randomMoveIndex(0, TRACK_LEN);
    uniform_real_distribution<double> uniformDouble(0, 1);
    auto bestScore = score(state, result, team);

    for (int iter = 0; iter < 30; iter++) {
        auto cur = getNextOrder();
        auto r = randomRobotIndex(rng);
        auto left = randomMoveIndex(rng);
        auto right = randomMoveIndex(rng);
        if (left > right) swap(left, right);

        auto angle = uniformDouble(rng) * M_PI * 2;
        auto x = cos(angle);
        auto y = 0.0;
        auto z = sin(angle);
        auto move = Move(Vec(x * MAX_ENTITY_SPEED, y * MAX_ENTITY_SPEED, z * MAX_ENTITY_SPEED), iter % 10 == 0 ? ROBOT_MAX_JUMP_SPEED : 0.0);

        for (size_t j = 0; j < team.size; j++) {
            for (size_t i = 0; i < TRACK_LEN; i++) {
                cur(j, i) = result(j, i);
            }
        }

        for (size_t i = left; i < right; i++) cur(r, i) = move;

        auto curScore = score(state, cur, team);
        if (curScore > bestScore) {
            bestScore = curScore;
            result = cur;
        }
    }

    for (size_t j = 0; j < team.size; j++) {
        size_t shift = TRACK_LEN * j;
        for (size_t i = 0; i < TRACK_LEN; i++) {
            bestOrder[shift + i] = result(j, i);
        }
    }
}

Solution::Solution(const Team& team) :
    team(team) {}

Move Solution::getMove(const State& state, const RobotState& me, int tick, int delta) {
    auto id = me.id;
    assert(isAlly(id)); // Otherwise support z-axis inversion

    if (tick != lastTick) {
        assert(delta == 0);
        compute(state, team, bestOrder, tick);
        lastTick = tick;
    }

    auto index = team.getIndex(id) * TRACK_LEN + delta;
    return index < bestOrder.size() ? bestOrder[index] : Move();
}
