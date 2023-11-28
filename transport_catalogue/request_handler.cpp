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
    
void Response(std::ostream& out, JsonManager& manager, renderer::MapBuilder& map_renderer, const transport::TransportCatalogue& catalogue){
    
    svg::Document svg_to_render = map_renderer.BuildSvg(catalogue);
    
    json::Print(manager.ProcessReqs(catalogue, svg_to_render), out);
}    
    
    
}// exit interface  


