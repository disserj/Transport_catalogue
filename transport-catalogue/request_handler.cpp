#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include <iostream>
namespace interface{
    
// Возвращает информацию о маршруте (запрос Bus)
std::optional<RequestHandler::BusStat> RequestHandler::GetBusStat(std::string_view bus_name) const{
    const std::shared_ptr<transport::Bus> bus = catalogue.FindRoute(std::string(bus_name));
    if(bus){
        int stops = 0;
        int unique = 0; 
        int len = 0;
        double curv = 0.0;
        std::tie(stops, unique, len, curv ) = catalogue.RouteInfo(std::string(bus_name));
        return BusStat{stops, unique, len, curv};
    }
    return {};
}
// Возвращает маршруты, проходящие через оcтановку:
const std::unordered_set<std::shared_ptr<transport::Bus>> RequestHandler::GetBusesByStop(std::string_view stop_name) const{
    return catalogue.FindStop(std::string(stop_name)).get()->GetBusPtrs();
}
  
svg::Document RequestHandler::RenderMap() const {  
   return map_renderer.BuildSvg(catalogue);
}
    
//вывод документа в поток:
void RequestHandler::Response(std::ostream& out, JsonManager& manager) const{
    json::Print(manager.ProcessReqs(catalogue, svg_to_render, router), out);
}
    
    
}// exit interface  


