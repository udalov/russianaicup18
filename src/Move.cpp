#include "Move.h"

#include <sstream>

using namespace std;

string Move::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << "move " << targetVelocity.toString() << " jump " << jumpSpeed;
    return out.str();
}
