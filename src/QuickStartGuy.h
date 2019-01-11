#pragma once

#include "Scenario.h"

template<bool isMe>
class QuickStartGuy : public Scenario {
    public:
        Move getMove(const State& state, const RobotState& me, int tick, size_t delta) override;

        ~QuickStartGuy() override = default;
};
