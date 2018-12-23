#pragma once

#include "Move.h"
#include "State.h"

Move quickStartMove(const State& state, const RobotState& me, int tick, int delta);
