#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace geo {

double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    const double dr = M_PI / 180.0;

    if (from == to) {
        return 0.0;
    }

    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * 6371000;
}

bool operator==(const Coordinates &lhs, const Coordinates &rhs) {
    return lhs.lat == rhs.lat && lhs.lng == rhs.lng;
}

bool operator!=(const Coordinates &lhs, const Coordinates &rhs) {
    return !(lhs == rhs);
}

}  // namespace geo
