#pragma once

#include "State.h"
#include "Vis.h"
#include "model/Rules.h"

void initializeSimulation(const model::Rules& rules, Vis& vis);

void simulate(State& state, int ticks);
