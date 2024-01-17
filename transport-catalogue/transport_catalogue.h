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
#include <memory>

namespace transport{


class TransportCatalogue{
  
  public:  //constr
    TransportCatalogue() = default;
  public: 

    void AddRoute(const std::string& num, const std::vector<std::string>& stops, const std::vector<std::shared_ptr<Stop>> ptrs, RouteType type);
    void AddStop(const std::string& name, geo::Coordinates& crd);
    void SetDistance( Stop* stop1,  Stop* stop2, int dist);
    
public: 
    const std::shared_ptr<Bus> FindRoute(const std::string& num) const;
    std::shared_ptr<Stop> FindStop(const std::string& name) const;
    std::tuple<int,int,int,double> RouteInfo(const std::string& name) const;
    const std::set<std::string> GetBusesNums(const std::string& name) const;
    int GetDistance(const Stop* stop1, const Stop* stop2) const;
    
    const std::map<std::string_view, std::shared_ptr<Stop>> GetStopCatalogue() const;
    const std::map<std::string_view, std::shared_ptr<Bus>> GetBusCatalogue() const;

    
private:
    
  std::deque<Bus> all_buses;
  std::deque<Stop> all_stops;
  std::unordered_map<std::string_view,std::shared_ptr<Stop>> stopname_to_stop; 
  std::unordered_map<std::string_view,std::shared_ptr<Bus>> busnum_to_bus;

};
    
} //exit transport    
