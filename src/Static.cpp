#include "Static.h"

#include <algorithm>
#include <vector>

using namespace model;
using namespace std;

namespace {
    const Rules *rules = nullptr;
    const Arena *arena = nullptr;

    vector<int> allyIds;
}

void initializeStatic(const Rules& initRules, const Game& game) {
    rules = &initRules;
    arena = &rules->arena;

    for (auto& robot : game.robots) {
        if (robot.is_teammate) allyIds.push_back(robot.id);
    }
}

const Rules& getRules() {
    return *rules;
}

const Arena& getArena() {
    return *arena;
}

bool isAlly(int robotId) {
    return find(allyIds.begin(), allyIds.end(), robotId) != allyIds.end();
}

bool isCaptain(int robotId) {
    return find_if(allyIds.begin(), allyIds.end(), [=](int id) { return id < robotId; }) == allyIds.end();
}
