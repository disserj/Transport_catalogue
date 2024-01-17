#pragma once
#include "json_reader.h"

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_set>

namespace interface{
    
    
class RequestHandler {
public:

struct BusStat{
    int stops_count;
    int unique_stops_cnt;
    int route_len;
    double curv;
};
    
    //конструктор
    RequestHandler(const transport::TransportCatalogue& cat, const renderer::MapBuilder& builder, const transport::RouteManager& route_manager )
    : catalogue(cat), map_renderer(builder), router(route_manager)
    {
        svg_to_render = map_renderer.BuildSvg(catalogue);
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(std::string_view bus_name) const;

    // Возвращает маршруты, проходящие через остановку:
    const std::unordered_set<std::shared_ptr<transport::Bus>> GetBusesByStop(std::string_view stop_name) const;
    
    //вывод документа в поток:
    void Response(std::ostream& out, JsonManager& manager) const;
      
    //метод возвращает карту:
    svg::Document RenderMap() const;
    

    
private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const transport::TransportCatalogue& catalogue;
    mutable renderer::MapBuilder map_renderer;
    const transport::RouteManager& router; 
    
    svg::Document svg_to_render;
};

} // exit interface  

