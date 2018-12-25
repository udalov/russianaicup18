#pragma once

#include "model/Game.h"

void initializeStatic(const model::Game& game);

bool isAlly(int robotId);

// Return true if this id is minimal among all ally robot ids
bool isCaptain(int robotId);
