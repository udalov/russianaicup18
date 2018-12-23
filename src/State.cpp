#include "State.h"

#include <sstream>

using namespace std;

string RobotState::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << "robot #" << id << " at=" << position.toString() << " v=" << velocity.toString() << " r=" << radius;
    return out.str();
}

string BallState::toString() const {
    ostringstream out;
    out << "ball at=" << position.toString() << " v=" << velocity.toString();
    return out.str();
}

const RobotState& State::findMe() const {
    for (auto& robot : robots) {
        if (robot.id == myId) return robot;
    }
    terminate();
}
