#pragma once

#include "Scenario.h"
#include "Team.h"
#include <array>

constexpr size_t TRACK_LEN = 50;
constexpr size_t ORDER_LEN = TRACK_LEN * 3;
constexpr int MICROTICKS = 5;

class Solution : public Scenario {
    private:
        const Team& team;
        int lastTick = -1;
        std::array<Move, ORDER_LEN> bestOrder;
    public:
        Solution(const Team& team);

        Move getMove(const State& state, const RobotState& me, int tick, int delta) override;

        ~Solution() {}
};
