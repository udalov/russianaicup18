#pragma once

#include "model/Game.h"
#include <string>

struct Team {
    int id0;
    int id1;
    int id2;
    int playerId;
    size_t size;
    bool allies;

    Team(int id0, int id1, int id2, int playerId, bool allies) :
        id0(id0), id1(id1), id2(id2), playerId(playerId), size(id2 == -1 ? 2 : 3), allies(allies) {}

    size_t getIndex(int id) const;
    int getId(size_t index) const;

    bool has(int id) const;

    std::string toString() const;

    static size_t NONE;
};

void initializeTeams(const model::Game& game);

const Team& getAllies();
const Team& getEnemies();
const Team& getTeam(int id);

bool isAlly(int id);

// Returns minimal id among all ally robot ids
int getCaptain();
