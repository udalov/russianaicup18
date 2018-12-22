#include "Vec.h"

using namespace std;

void Vec::clamp(double length) {
    auto len = this->len();
    if (len > length) {
        x *= length / len;
        y *= length / len;
        z *= length / len;
    }
}

string Vec::toString() const {
    return string("(") + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
}
