#pragma once

namespace geo {

struct Coordinates {
    double lat; // Широта
    double lng; // Долгота
};

bool operator==(const Coordinates& lhs, const Coordinates& rhs);

bool operator!=(const Coordinates& lhs, const Coordinates& rhs);

double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo
