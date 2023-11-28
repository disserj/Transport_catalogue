#pragma once

#include "geo.h"
#include "domain.h"

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


class TransportCatalogue{
  
  public:  //constr
    TransportCatalogue() = default;
  public: 

    void AddRoute(const std::string& num, const std::vector<std::string>& stops, const std::vector<Stop*> ptrs, bool flag);
    void AddStop(const std::string& name, geo::Coordinates& crd);
    void SetDistance( Stop* stop1,  Stop* stop2, int dist);
    
public: 
    const Bus* FindRoute(const std::string& num) const;
    Stop* FindStop(const std::string& name) const;
    std::tuple<int,int,int,double> RouteInfo(const std::string& name) const;
    const std::set<std::string> GetBusesNums(const std::string& name) const;
    int GetDistance(const Stop* stop1, const Stop* stop2) const;
    
    const std::map<std::string_view, Stop*> GetStopCatalogue() const;
    const std::map<std::string_view, Bus*> GetBusCatalogue() const;
    const std::unordered_set<Stop*> GetListOfUniqueStops() const;
    
    
private:
    
    std::unordered_map<std::string_view,Bus*> busnum_to_bus;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    
    std::deque<Bus> all_buses;
    std::deque<Stop> all_stops;

};
    
} //exit transport    
