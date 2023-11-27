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




class TransportCatalogue{
  
  public:  //constr
    TransportCatalogue() = default;
  public: 
    struct Stop{
    Stop() = default;
    std::string name;
    geo::Coordinates point;
    std::unordered_map<std::string, int> distances;
    };

    struct Bus{
        Bus() = default;
        std::string num;
        std::vector<std::string_view> stops;
        bool cycle = true;
        const std::vector<Stop*> GetStopsPtrs(const TransportCatalogue& catalogue) const{
            std::vector<Stop*> ptrs;
            std::transform(stops.cbegin(), stops.cend(),std::back_inserter(ptrs),[&catalogue](const std::string_view& stop){
                return catalogue.FindStop((std::string)stop);
            });
        return ptrs;
        }
    };

    void AddRoute(const std::string& num, const std::vector<std::string_view>& stops, bool flag);
    void AddStop(const std::string& name, geo::Coordinates& crd);
    void SetDistance( Stop* stop1,  Stop* stop2, int dist);
    
public: 
    const TransportCatalogue::Bus* FindRoute(const std::string& num) const;
    TransportCatalogue::Stop* FindStop(const std::string& name) const;
    std::tuple<int,int,int,double> RouteInfo(const std::string& name) const;
    const std::set<std::string> GetBusesNums(const std::string& name) const;
    int GetDistance(const Stop* stop1, const Stop* stop2) const;
    
    const std::map<std::string_view,  TransportCatalogue::Stop*> GetStopCatalogue() const;
    const std::map<std::string_view,  TransportCatalogue::Bus*> GetBusCatalogue() const;
    const std::unordered_set<TransportCatalogue::Stop*> GetListOfUniqueStops() const;
    
    
private:
    
    std::unordered_map<std::string_view, TransportCatalogue::Bus*> busnum_to_bus;
    std::unordered_map<std::string_view,  TransportCatalogue::Stop*> stopname_to_stop;
    
    std::deque<Bus> all_buses;
    std::deque<Stop> all_stops;

};
    
} //exit transport    
