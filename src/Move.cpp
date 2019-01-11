#include "Move.h"

#include <sstream>

using namespace std;

string Move::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << "move " << targetVelocity.toString();
    if (jumpSpeed > 0.0) {
        out << " jump " << jumpSpeed;
    }
    if (useNitro) {
        out << " (nitro)";
    }
    return out.str();
}
