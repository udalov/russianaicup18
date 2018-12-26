#pragma once

#include "Move.h"
#include "State.h"

class Scenario {
    public:
        virtual Move getMove(const State& state, const RobotState& me, int tick, int delta) = 0;

        virtual ~Scenario() {}
};
