#include "State.h"

#include "Static.h"
#include <sstream>

using namespace std;

string RobotState::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << (isAlly(id) ? "ally" : "enemy") << " #" << id << " at=" << position.toString() << " v=" << velocity.toString() << " r=" << radius << (touch ? " T" : "");
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
