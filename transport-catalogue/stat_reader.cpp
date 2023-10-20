
#include "stat_reader.h"

namespace transport{

void Response(std::ostream& out,RequestQueue& request, const TransportCatalogue& catalogue){
    std::string num;
    std::string name;
    for(const auto& req : request.ToResponse){
        if(!req.empty()){
            if(req.substr(0,4) == "Stop"){
                name = req.substr(req.find_first_of(' ')+1);
                if(catalogue.FindStop(name)){
                    StopResponse(out,{name,true},catalogue);
                }else{
                    StopResponse(out,{name,false},catalogue);
                }
            } else if(req.substr(0,3) == "Bus"){
                num = req.substr(req.find_first_of(' ')+1);
                if(catalogue.FindRoute(num)){
                    BusResponse(out,{num,true},catalogue);
                }else{
                    BusResponse(out,{num,false},catalogue);
                }
            }
        }
    }
}


void BusResponse(std::ostream& out, const std::pair<std::string,bool>& bus, const TransportCatalogue& catalogue){
        if(bus.second){
            int stops = 0;
            int unique = 0; 
            int len = 0;
            double curv = 0.0;
            std::tie(stops, unique, len, curv ) = catalogue.RouteInfo(bus.first);
                    
            out << "Bus " << bus.first << ": " << stops << " stops on route, "
            << unique << " unique stops, " << len << " route length, "
            << curv <<" curvature\n";
                        
        }else {
            out << "Bus " << bus.first << ": not found\n";
        }
}

void StopResponse(std::ostream& out, const std::pair<std::string,bool>& stop, const TransportCatalogue& catalogue){
        if(stop.second){
            std::set<std::string>buses_list;
            buses_list = catalogue.GetBusesNums(stop.first);
            if(buses_list.empty()){
                out << "Stop " << stop.first << ": " << "no buses\n";
            }else{
                out << "Stop " << stop.first << ": buses ";
                for(const auto& bus: buses_list){
                    out << bus << " ";
                }
                    out << "\n";
            }
        }else{
            out << "Stop " << stop.first << ": " << "not found\n";
        }
}
    
}//exit transport


