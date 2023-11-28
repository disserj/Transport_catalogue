#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>


namespace transport{
    
struct Stop{
    Stop() = default;
    std::string name;
    geo::Coordinates point;
    std::unordered_map<std::string, int> distances;
};

struct Bus{
    Bus() = default;
    std::string num;
    std::vector<std::string> stops;
    std::vector<Stop*> stop_ptrs;
    bool cycle = true;
    const std::vector<Stop*> GetStopsPtrs() const{
        return stop_ptrs;
    }
};

}// exit transport