#include "Team.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace model;
using namespace std;

namespace {
    const Team *allies = nullptr;
    const Team *enemies = nullptr;
    int allyBits;
    int enemyBits;
}

size_t Team::getIndex(int id) const {
    if (id == id0) return 0;
    if (id == id1) return 1;
    if (id == id2) return 2;
    return NONE;
}

int Team::getId(size_t index) const {
    switch (index) {
        case 0: return id0;
        case 1: return id1;
        case 2: return id2;
        default: return -1;
    }
}

bool Team::has(int id) const {
    return id == id0 || id == id1 || id == id2;
}

string Team::toString() const {
    ostringstream out;
    out << (allies ? "allies" : "enemies") << " (" << id0 << ", " << id1;
    if (id2 != -1) {
        out << ", " << id2;
    }
    out << ")";
    return out.str();
}

void initializeTeams(const Game& game) {
    vector<int> allyIds;
    vector<int> enemyIds;
    int allyPlayerId = -1;
    int enemyPlayerId = -1;
    allyBits = 0;
    enemyBits = 0;
    for (auto& robot : game.robots) {
        auto id = robot.id;
        if (robot.is_teammate) {
            allyIds.push_back(id);
            allyPlayerId = robot.player_id;
            allyBits |= (1 << id);
        } else {
            enemyIds.push_back(id);
            enemyPlayerId = robot.player_id;
            enemyBits |= (1 << id);
        }
    }

    sort(allyIds.begin(), allyIds.end());
    sort(enemyIds.begin(), enemyIds.end());

    allies = new Team(allyIds.at(0), allyIds.at(1), allyIds.size() == 3 ? allyIds.at(2) : -1, allyPlayerId, true);
    enemies = new Team(enemyIds.at(0), enemyIds.at(1), enemyIds.size() == 3 ? enemyIds.at(2) : -1, enemyPlayerId, false);
}

const Team& getAllies() {
    return *allies;
}

const Team& getEnemies() {
    return *enemies;
}

const Team& getTeam(int id) {
    return allies->has(id) ? *allies : *enemies;
}

bool isAlly(int id) {
    return allyBits & (1 << id);
}

int getCaptain() {
    return allies->id0;
}
