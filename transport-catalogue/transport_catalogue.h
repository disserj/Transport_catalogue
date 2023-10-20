#pragma once

#include "geo.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <optional>
#include <algorithm>

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
    bool cycle = true;
    
};


class TransportCatalogue{
  private:
    std::deque<Stop> all_stops;
    std::deque<Bus> all_buses;
    std::unordered_map<std::string_view, Bus*> busnum_to_bus;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    
    
  public:  //constr
    TransportCatalogue() = default;
    
  public: 
    void AddRoute(const std::string& num, std::vector<std::string> stops, bool flag);
    void AddStop(const std::string& name, geo::Coordinates& crd);
    void SetDistance( Stop* stop1,  Stop* stop2, int dist);
    
  public: 
    const Bus* FindRoute(const std::string& num) const;
    Stop* FindStop(const std::string& name) const;
    std::tuple<int,int,int,double> RouteInfo(const std::string& name) const;
    const std::set<std::string> GetBusesNums(const std::string& name) const;
    int GetDistance(const Stop* stop1, const Stop* stop2) const;
};
    
} //exit transport    
