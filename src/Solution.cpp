#include "Solution.h"

#include "Const.h"
#include "Simulation.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

struct ScoredOrder;

namespace {
    mt19937 rng(42);

    vector<Move> orders(2000 * ORDER_LEN);
    size_t nextOrderIndex = 0;

    vector<ScoredOrder> results;
    vector<ScoredOrder> personalBests;
    uniform_int_distribution<size_t> randomRobotIndex(0, 0);
    uniform_int_distribution<size_t> randomMoveIndex(0, TRACK_LEN);
    uniform_real_distribution<double> uniformDouble(0, 1);
}

Move& Order::get(size_t robotIndex, int delta) {
    return orders[index + robotIndex * TRACK_LEN + delta];
}

Order getNextOrder() {
    auto result = Order(nextOrderIndex);
    nextOrderIndex += ORDER_LEN;
    if (nextOrderIndex >= orders.size()) nextOrderIndex = 0;
    return result;
}

Order newEmptyOrder(size_t teamSize) {
    auto result = getNextOrder();
    for (size_t j = 0; j < teamSize; j++) {
        for (size_t i = 0; i < TRACK_LEN; i++) {
            result(j, i) = Move();
        }
    }
    return result;
}

struct ScoredOrder {
    double score;
    Order order;

    ScoredOrder(double score, Order order) : score(score), order(order) {}

    bool operator<(const ScoredOrder& other) const {
        return score < other.score;
    }
};

struct TickData {
    const Team& team;
    Scenario& enemyStrategy;
    int tick;

    TickData(const Team& team, Scenario& enemyStrategy, int tick) : team(team), enemyStrategy(enemyStrategy), tick(tick) {}
};

double scoreDefense(const RobotState& robot, const State& state) {
    // auto target = Vec(state.ball.position.x / 3, 0, -ARENA_D / 2 - BALL_RADIUS);
    auto target = Vec(0, 0, -ARENA_D / 2);
    return -robot.position.distance(target) - sqr(sqr(robot.position.y));
}

double scoreAttack(const RobotState& robot, const State& state) {
    // return -robot.position.distance(state.ball.position);
    Vec myPosXZ = Vec(robot.position.x, 0, robot.position.z);
    Vec ballPosXZ = Vec(state.ball.position.x, 0, state.ball.position.z - BALL_RADIUS - robot.radius);
    return -myPosXZ.distance(ballPosXZ);
}

double scoreState(const State& state) {
    const RobotState *robot0 = nullptr;
    const RobotState *robot1 = nullptr;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) {
            robot1 = robot0;
            robot0 = &robot;
        }
    }
    auto& ball = state.ball;

    auto score = 0.0;

    score += 10 * ball.position.z;

    auto wantedBallVelocity = Vec(0, ARENA_GH/2, ARENA_D) - ball.position;
    wantedBallVelocity *= (MAX_ENTITY_SPEED / 3 / wantedBallVelocity.len());
    score -= 100 * ball.velocity.sqrDist(wantedBallVelocity);
    score += 100000 * state.goal;

    return score + max(
        scoreDefense(*robot0, state) + scoreAttack(*robot1, state),
        scoreDefense(*robot1, state) + scoreAttack(*robot0, state)
    );
}

ScoredOrder scoreOrder(State state, const TickData& data, const Order& order) {
    auto ans = 0.0;

    simulate(state, TRACK_LEN, MICROTICKS, nullptr, [&data, &order, &ans](const State& state, const RobotState& robot, int delta) {
        auto index = data.team.getIndex(robot.id);
        if (index == Team::NONE) {
            return data.enemyStrategy.getMove(state, robot, data.tick, delta);
        }

        if (index == 0 && (TRACK_LEN - 1 - delta) % SCORE_EACH_NTH == 0) {
            ans += scoreState(state); // * (TRACK_LEN - delta) / TRACK_LEN;
        }

        return order(index, delta);
    });

    return ScoredOrder(ans, order);
}

Move generateRandomMove() {
    auto angle = uniformDouble(rng) * M_PI * 2;
    auto x = cos(angle);
    auto z = sin(angle);
    return Move(Vec(x * MAX_ENTITY_SPEED, 0.0, z * MAX_ENTITY_SPEED), 0.0);
}

template<size_t bestOrdersSize>
void compute(const State& state, const TickData& data, array<Order, bestOrdersSize>& bestOrders) {
    auto teamSize = data.team.size;

    results.clear();
    for (auto& bestOrder : bestOrders) {
        for (size_t j = 0; j < teamSize; j++) {
            for (size_t i = 1; i < TRACK_LEN; i++) {
                bestOrder(j, i - 1) = bestOrder(j, i);
            }
        }
        results.push_back(scoreOrder(state, data, bestOrder));
    }

    constexpr auto PERSONAL_BEST = 5;
    personalBests.clear();
    for (size_t j = 0; j < teamSize; j++) {
        auto personalBest = *results.begin();
        for (int iter = 0; iter < PERSONAL_BEST; iter++) {
            auto cur = getNextOrder();

            auto move = generateRandomMove();
            for (size_t i = 0; i < TRACK_LEN; i++) cur(j, i) = move;
            for (size_t k = 0; k < teamSize; k++) if (k != j) {
                for (size_t i = 0; i < TRACK_LEN; i++) cur(k, i) = Move();
            }

            auto scored = scoreOrder(state, data, cur);
            personalBest = max(personalBest, scored);
            results.push_back(scored);
        }
        personalBests.push_back(personalBest);
    }

    {
        auto cur = getNextOrder();
        for (size_t j = 0; j < teamSize; j++) {
            for (size_t i = 0; i < TRACK_LEN; i++) cur(j, i) = personalBests[j].order(j, i);
        }
        results.push_back(scoreOrder(state, data, cur));
    }

    make_heap(results.begin(), results.end());

    for (int iter = 0; iter < 20; iter++) {
        auto cur = getNextOrder();

        auto r = randomRobotIndex(rng);
        auto left = randomMoveIndex(rng);
        auto right = TRACK_LEN; // randomMoveIndex(rng);
        if (left > right) swap(left, right);

        auto move = generateRandomMove();
        if (iter % 10 == 0) {
            move.jumpSpeed = ROBOT_MAX_JUMP_SPEED;
        }

        for (size_t j = 0; j < teamSize; j++) {
            for (size_t i = 0; i < TRACK_LEN; i++) {
                cur(j, i) = results.front().order(j, i);
            }
        }

        for (size_t i = left; i < right; i++) cur(r, i) = move;

        results.push_back(scoreOrder(state, data, cur));
        push_heap(results.begin(), results.end());
    }

    sort(results.rbegin(), results.rend());

    for (size_t i = 0; i < bestOrders.size(); i++) {
        auto& scored = i < results.size() ? results[i] : results.back();
        bestOrders[i] = scored.order;
    }
}

Solution::Solution(const Team& team, Scenario& enemyStrategy) : team(team), enemyStrategy(enemyStrategy) {
    auto order = newEmptyOrder(team.size);
    for (auto& bestOrder : bestOrders) {
        bestOrder = order;
    }
}

Move Solution::getMove(const State& state, const RobotState& me, int tick, int delta) {
    auto id = me.id;
    assert(isAlly(id)); // Otherwise support z-axis inversion

    if (tick == 0) {
        if (team.size == 2) {
            randomRobotIndex = uniform_int_distribution<size_t>(0, 1);
        } else if (team.size == 3) {
            randomRobotIndex = uniform_int_distribution<size_t>(0, 2);
        }
    }

    if (tick != previousTick) {
        assert(delta == 0);
        previousTick = tick;

        if (tick == lastGoalTick) {
            auto order = newEmptyOrder(team.size);
            for (auto& bestOrder : bestOrders) {
                bestOrder = order;
            }
        } else if (tick >= lastGoalTick + RESET_TICKS - 1) {
            compute(state, TickData(team, enemyStrategy, tick), bestOrders);
        }
    }

    return static_cast<size_t>(delta) < TRACK_LEN ? bestOrders[0](team.getIndex(id), delta) : Move();
}
