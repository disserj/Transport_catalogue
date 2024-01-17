#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>
#include <unordered_set>

namespace transport{
    
enum class RouteType {
    BLANK,
    LINEAR,
    CIRCLE,
};    

struct Bus;
struct Stop;    
    
struct Stop{
    Stop() = default;
    std::string name;
    geo::Coordinates point;
    std::unordered_map<std::string, int> distances;
    std::unordered_set<std::shared_ptr<Bus>> bus_ptrs;
    const std::unordered_set<std::shared_ptr<Bus>> GetBusPtrs() const{
        return bus_ptrs;
    }
};

struct Bus{
    Bus() = default;
    std::string num;
    std::vector<std::string> stops;
    std::vector<std::shared_ptr<Stop>> stop_ptrs;
    RouteType type = RouteType::BLANK;
    const std::vector<std::shared_ptr<Stop>> GetStopsPtrs() const{
        return stop_ptrs;
    }
};    
    

}// exit transport