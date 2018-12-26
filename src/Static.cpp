#include "Static.h"

#include <algorithm>

using namespace model;
using namespace std;

namespace {
    int allyId1 = -1;
    int allyId2 = -1;
    int allyId3 = -1;
}

void initializeStatic(const Game& game) {
    if (allyId1 != -1) return;

    for (auto& robot : game.robots) {
        if (robot.is_teammate) {
            if (allyId1 == -1) allyId1 = robot.id;
            else if (allyId2 == -1) allyId2 = robot.id;
            else if (allyId3 == -1) allyId3 = robot.id;
            else terminate();
        }
    }

    if (allyId1 > allyId2) swap(allyId1, allyId2);
    if (allyId3 != -1) {
        if (allyId1 > allyId3) swap(allyId1, allyId3);
        if (allyId2 > allyId3) swap(allyId2, allyId3);
    }
}

bool isAlly(int robotId) {
    return robotId == allyId1 || robotId == allyId2 || robotId == allyId3;
}

int getCaptain() {
    return allyId1;
}
