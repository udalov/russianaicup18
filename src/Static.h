#pragma once

#include "model/Game.h"
#include "model/Rules.h"

void initializeStatic(const model::Rules& rules, const model::Game& game);

const model::Rules& getRules();

const model::Arena& getArena();

bool isAlly(int robotId);

// Return true if this id is minimal among all ally robot ids
bool isCaptain(int robotId);
