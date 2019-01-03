#pragma once

#include "Move.h"
#include "State.h"

class Scenario {
    protected:
        int lastGoalTick = -31337;

    public:
        void goalScored(int tick) {
            lastGoalTick = tick;
        }

        virtual Move getMove(const State& state, const RobotState& me, int tick, int delta) = 0;

        virtual ~Scenario() {}
};
