#pragma once

#include "Move.h"
#include "State.h"
#include "model/Rules.h"

void updateRobot(RobotState& robot, double deltaTime, const model::Rules& rules, const Move& move);

bool collideRobotArena(RobotState& robot, double deltaTime, const model::Rules& rules, Vec& result);
