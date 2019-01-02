#include "Vec.h"

#include <sstream>

using namespace std;

void Vec::clamp(double length) {
    if (abs(x) + abs(y) + abs(z) > length) {
        auto len = this->len();
        if (len > length) {
            x *= length / len;
            y *= length / len;
            z *= length / len;
        }
    }
}

string Vec::toString() const {
    ostringstream out;
    out.precision(3);
    out << fixed << "(" << x << ", " << y << ", " << z << ")";
    return out.str();
}
