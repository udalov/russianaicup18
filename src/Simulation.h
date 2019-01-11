#pragma once

#include "Move.h"
#include "State.h"
#include "Vis.h"
#include <functional>

void simulate(State& state, int ticks, int microticks, Vis *vis, const std::function<Move(const State&, const RobotState&, size_t)>& getMove);
