#pragma once

#include "Scenario.h"
#include "Team.h"
#include <array>

constexpr size_t TRACK_LEN = 80;
constexpr size_t ORDER_LEN = TRACK_LEN * 3;
constexpr size_t BEST_ORDERS_CACHE_SIZE = 10;
constexpr int SCORE_EACH_NTH = 5;
constexpr int MICROTICKS = 4;

struct Order {
    size_t index;

    Order() : index(static_cast<size_t>(-1)) {}
    explicit Order(size_t index) : index(index) {}

    Move& get(size_t robotIndex, size_t delta);

    Move& operator()(size_t robotIndex, size_t delta) { return get(robotIndex, delta); }
    const Move& operator()(size_t robotIndex, size_t delta) const { return const_cast<Order *>(this)->get(robotIndex, delta); }
};

class Solution : public Scenario {
    private:
        const Team& team;
        Scenario& enemyStrategy;

        int previousTick = -1;
        std::array<Order, BEST_ORDERS_CACHE_SIZE> bestOrders;
    public:
        Solution(const Team& team, Scenario& enemyStrategy);

        Move getMove(const State& state, const RobotState& me, int tick, size_t delta) override;

        ~Solution() override = default;
};
