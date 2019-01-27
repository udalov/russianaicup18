#include "Solution.h"

#include "Const.h"
#include "Simulation.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

struct ScoredOrder;

namespace {
    constexpr auto NITRO_CONSIDERATION_THRESHOLD = 10.0;

    mt19937 rng(42);

    vector<Move> orders(2000 * ORDER_LEN);
    size_t nextOrderIndex = 0;

    vector<ScoredOrder> results;
    vector<ScoredOrder> personalBests;
    uniform_int_distribution<size_t> randomRobotIndex(0, 0);
    uniform_int_distribution<size_t> randomMoveIndex(0, TRACK_LEN);
    uniform_real_distribution<double> uniformDouble(0, 1);
}

Config Config::DEFAULT = [](){
    Config result;
    result.scoreEachNth = 5;
    result.personalBestRandomOrders = 15;
    result.personalBestWithNitro = 10;
    result.randomSuffixOrders = 20;
    result.jumpEveryNthRandomSuffixOrder = 10;
    return result;
}();

Move& Order::get(size_t robotIndex, size_t delta) {
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
    const Config& config;

    TickData(const Team& team, Scenario& enemyStrategy, int tick, const Config& config) :
        team(team), enemyStrategy(enemyStrategy), tick(tick), config(config) {}
};

double max6(double a0, double a1, double a2, double a3, double a4, double a5) {
    return max(max(max(a0, a1), max(a2, a3)), max(a4, a5));
}

double scoreDefense2(const RobotState& robot) {
    // constexpr auto target = Vec(ball.position.x / 3, 0, -ARENA_D / 2 - BALL_RADIUS);
    constexpr auto target = Vec(0, 0, -ARENA_D / 2);
    return -robot.position.distance(target) - sqr(sqr(robot.position.y));
}

double scoreAttack2(const RobotState& robot, const BallState& ball) {
    // return -robot.position.distance(ball.position);
    Vec myPosXZ = Vec(robot.position.x, 0, robot.position.z);
    Vec ballPosXZ = Vec(ball.position.x, 0, ball.position.z - BALL_RADIUS - robot.radius);
    return -myPosXZ.distance(ballPosXZ);
}

double scoreDefense3(const RobotState& robot) {
    return scoreDefense2(robot);
}

double scoreMidfield3(const RobotState& robot, const BallState& ball) {
    return scoreAttack2(robot, ball);
}

double scoreAttack3(const RobotState& robot, const BallState& ball) {
    return scoreAttack2(robot, ball);
}

double scoreState(const State& state, const RobotState& robot0, const RobotState& robot1, const RobotState *robot2) {
    auto& ball = state.ball;

    auto score = 0.0;

    score += 10 * ball.position.z;

    auto wantedBallVelocity = Vec(0, ARENA_GH/2, ARENA_D) - ball.position;
    wantedBallVelocity *= (MAX_ENTITY_SPEED / 3 / wantedBallVelocity.len());
    score -= 100 * ball.velocity.sqrDist(wantedBallVelocity);
    score += 100000 * state.goal;

    if (robot2 != nullptr) {
        score += (robot0.nitro + robot1.nitro + robot2->nitro) * 2./3;
    } else {
        score += robot0.nitro + robot1.nitro;
    }

    auto positioning = robot2 != nullptr ?
        max6(
            scoreDefense3(robot0) + scoreMidfield3(robot1, ball) + scoreAttack3(*robot2, ball),
            scoreDefense3(robot0) + scoreMidfield3(*robot2, ball) + scoreAttack3(robot1, ball),
            scoreDefense3(robot1) + scoreMidfield3(robot0, ball) + scoreAttack3(*robot2, ball),
            scoreDefense3(robot1) + scoreMidfield3(*robot2, ball) + scoreAttack3(robot0, ball),
            scoreDefense3(*robot2) + scoreMidfield3(robot0, ball) + scoreAttack3(robot1, ball),
            scoreDefense3(*robot2) + scoreMidfield3(robot1, ball) + scoreAttack3(robot0, ball)
        ) : max(
            scoreDefense2(robot0) + scoreAttack2(robot1, ball),
            scoreDefense2(robot1) + scoreAttack2(robot0, ball)
        );

    return score + positioning;
}

ScoredOrder scoreOrder(State state, const TickData& data, const Order& order) {
    const RobotState *robot0 = nullptr;
    const RobotState *robot1 = nullptr;
    const RobotState *robot2 = nullptr;
    for (auto& robot : state.robots) {
        if (isAlly(robot.id)) {
            robot2 = robot1;
            robot1 = robot0;
            robot0 = &robot;
        }
    }
    assert((robot2 != nullptr) == (data.team.size == 3));

    auto ans = 0.0;

    simulate(state, TRACK_LEN, MICROTICKS, nullptr, [&data, &order, &ans, robot0, robot1, robot2](const State& state, const RobotState& robot, size_t delta) {
        auto index = data.team.getIndex(robot.id);
        if (index == Team::NONE) {
            return data.enemyStrategy.getMove(state, robot, data.tick, delta);
        }

        if (index == 0 && (TRACK_LEN - 1 - delta) % data.config.scoreEachNth == 0) {
            ans += scoreState(state, *robot0, *robot1, robot2); // * (TRACK_LEN - delta) / TRACK_LEN;
        }

        return order(index, delta);
    });

    return ScoredOrder(ans, order);
}

Move generateRandomMove() {
    auto angle = uniformDouble(rng) * M_PI * 2;
    auto x = cos(angle);
    auto z = sin(angle);
    return Move(Vec(x * MAX_ENTITY_SPEED, 0.0, z * MAX_ENTITY_SPEED), 0.0, false);
}

Move generateRandomNitroMove() {
    auto u = uniformDouble(rng) * M_PI * 2;
    auto h = uniformDouble(rng) * 2 - 1;
    auto r = sqrt(1 - sqr(h));
    return Move(Vec(cos(u) * r, sin(u) * r, h) * MAX_ENTITY_SPEED, 0.0, true);
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

    personalBests.clear();
    for (size_t j = 0; j < teamSize; j++) {
        auto personalBest = *results.begin();

        size_t withNitro =
            state.findRobotById(data.team.getId(j)).nitro > NITRO_CONSIDERATION_THRESHOLD
            ? data.config.personalBestWithNitro
            : 0;

        for (size_t iter = 0; iter < data.config.personalBestRandomOrders + withNitro; iter++) {
            auto cur = getNextOrder();

            auto move1 = generateRandomMove();
            auto move2 = iter < data.config.personalBestRandomOrders ? generateRandomMove() : generateRandomNitroMove();
            auto change = randomMoveIndex(rng);
            for (size_t i = 0; i < TRACK_LEN; i++) cur(j, i) = i < change ? move1 : move2;
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

    for (size_t iter = 0; iter < data.config.randomSuffixOrders; iter++) {
        auto cur = getNextOrder();

        auto r = randomRobotIndex(rng);
        auto left = randomMoveIndex(rng);
        auto right = TRACK_LEN; // randomMoveIndex(rng);
        if (left > right) swap(left, right);

        auto move = generateRandomMove();
        if (iter % data.config.jumpEveryNthRandomSuffixOrder == 0) {
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

Move Solution::getMove(const State& state, const RobotState& me, int tick, size_t delta) {
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
            compute(state, TickData(team, enemyStrategy, tick, Config::DEFAULT), bestOrders);
        }
    }

    return static_cast<size_t>(delta) < TRACK_LEN ? bestOrders[0](team.getIndex(id), delta) : Move();
}
