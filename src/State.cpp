#include "State.h"

#include "Team.h"
#include <sstream>

using namespace std;

size_t getClosestNitroPackIndex(double x, double z) {
    if (x < 0) {
        return z < 0 ? 0 : 1;
    } else {
        return z < 0 ? 2 : 3;
    }
}

string RobotState::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << (isAlly(id) ? "ally" : "enemy") << " #" << id << " at=" << position.toString() << " v=" << velocity.toString() << " n=" << nitro << (touch ? " T" : "");
    return out.str();
}

string BallState::toString() const {
    ostringstream out;
    out << "ball at=" << position.toString() << " v=" << velocity.toString();
    return out.str();
}

const RobotState& State::findRobotById(int id) const {
    for (auto& robot : robots) {
        if (robot.id == id) return robot;
    }
    terminate();
}
