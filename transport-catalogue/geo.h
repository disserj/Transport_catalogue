#pragma once

#include <cmath>

namespace geo{

const int RADIUS = 6371000;

struct Coordinates {
    double lat;
    double lng;
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

 double ComputeDistance(Coordinates from, Coordinates to);
}//exit geo
