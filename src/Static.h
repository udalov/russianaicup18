#pragma once

#include "model/Game.h"

void initializeStatic(const model::Game& game);

bool isAlly(int robotId);

// Returns minimal id among all ally robot ids
int getCaptain();
