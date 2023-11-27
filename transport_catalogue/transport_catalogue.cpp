
#include "transport_catalogue.h"
#include <iostream>

namespace transport{

void TransportCatalogue::SetDistance(Stop* stop1, Stop* stop2, int distance) {
    stop1->distances[stop2->name] = distance;
}

int TransportCatalogue::GetDistance(const Stop* stop1, const Stop* stop2) const {
    if (stop1->distances.count(stop2->name)) return stop1->distances.at(stop2->name);
    else if (stop2->distances.count(stop1->name)) return stop2->distances.at(stop1->name);
    return 0;
}


void TransportCatalogue::AddRoute(const std::string& num, const std::vector<std::string_view>& stops, bool flag){
     all_buses.push_back({ num, stops, flag });
     busnum_to_bus[all_buses.back().num] = &all_buses.back();
        
}
void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates& point){    
     all_stops.push_back({ name, point, {}});   
     stopname_to_stop[all_stops.back().name] = &all_stops.back();
}

const TransportCatalogue::Bus* TransportCatalogue::FindRoute(const std::string& num) const{
     if(!busnum_to_bus.count(num)) return nullptr;
     return busnum_to_bus.at(num);
}
TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name) const{
     if(!stopname_to_stop.count(name)) return nullptr;
     return stopname_to_stop.at(name);
}
std::tuple<int,int,int,double> TransportCatalogue::RouteInfo(const std::string& num) const{
     int stops_count = 0;
     int unique_stops_cnt = 0;
     int route_len = 0;
     double curv = 0.0;
     const Bus* bus = FindRoute(num);
        
     //считаем кол-во
        
     if (bus->cycle){
        stops_count = bus->stops.size();
     }else{
        stops_count = bus->stops.size() * 2 - 1;
     }
     //считаем уникальные
     std::unordered_set<std::string_view> unique;
     for (const auto& stop : busnum_to_bus.at(num)->stops) {
        unique.insert(stop);
     }    
     unique_stops_cnt = unique.size();

    //считаем длину и курватуру
     double len = 0.0;

     for (size_t i = 1; i < bus->stops.size(); ++i) {
        auto from = stopname_to_stop.find(bus->stops[i-1])->second;
        auto to = stopname_to_stop.find(bus->stops[i])->second;
        if (bus->cycle) {
            route_len += GetDistance(from, to);
            len += geo::ComputeDistance(stopname_to_stop.find(bus->stops[i-1])->second->point,
                stopname_to_stop.find(bus->stops[i])->second->point);
        } else {
            route_len += GetDistance(from, to) + GetDistance(to, from);
            len += ComputeDistance(stopname_to_stop.find(bus->stops[i-1])->second->point,
                stopname_to_stop.find(bus->stops[i])->second->point) * 2;
        }
    }
    if (bus->cycle) {
        auto from = stopname_to_stop.find(bus->stops[bus->stops.size()-1])->second;
        auto to = stopname_to_stop.find(bus->stops[0])->second;
        route_len += GetDistance(from, to);
        len += geo::ComputeDistance(from->point, to->point);
    }
    curv = route_len/len;
        
    return {stops_count, unique_stops_cnt, route_len, curv};
}

const std::set<std::string> TransportCatalogue::GetBusesNums(const std::string& name) const{
    std::set<std::string> res;
    for(auto& bus : all_buses){
        if(std::count(bus.stops.begin(),bus.stops.end(),name) != 0) res.insert(bus.num);
    }
    return res;    
}
    
const std::map<std::string_view, TransportCatalogue::Bus*> TransportCatalogue::GetBusCatalogue() const{
    std::map<std::string_view, TransportCatalogue::Bus*> sort_nums;
    for (const auto& bus : busnum_to_bus) {
        sort_nums.emplace(bus);
    }
    return sort_nums;
}

const std::map<std::string_view,  TransportCatalogue::Stop*> TransportCatalogue::GetStopCatalogue() const{
    std::map<std::string_view,  Stop*> sort_stops;
    for (const auto& bus : stopname_to_stop) {
        sort_stops.emplace(bus);
    }
    return sort_stops;
}  
    
const std::unordered_set<TransportCatalogue::Stop*> TransportCatalogue::GetListOfUniqueStops() const{
    std::unordered_set<TransportCatalogue::Stop*> unique_stops;
    for (const auto& [name, stop_ptr] : GetStopCatalogue()) {
        unique_stops.insert(stop_ptr);
    }
    return unique_stops;
}    
    
} //exit transport    
