#include "Static.h"

#include <algorithm>
#include <vector>

using namespace model;
using namespace std;

namespace {
    vector<int> allyIds;
}

void initializeStatic(const Game& game) {
    for (auto& robot : game.robots) {
        if (robot.is_teammate) allyIds.push_back(robot.id);
    }
}

bool isAlly(int robotId) {
    return find(allyIds.begin(), allyIds.end(), robotId) != allyIds.end();
}

bool isCaptain(int robotId) {
    return find_if(allyIds.begin(), allyIds.end(), [robotId](int id) { return id < robotId; }) == allyIds.end();
}
