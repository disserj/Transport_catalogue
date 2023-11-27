#pragma once
#include "transport_catalogue.h"
#include "json_reader.h"
#include "svg.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

namespace interface{
    
class RequestHandler {
public:
    RequestHandler(const transport::TransportCatalogue& cat, renderer::MapBuilder& builder)
        : catalogue(cat)
        , map_renderer(builder)
    {}
    
    void Response(std::ostream& out, JsonManager& manager);

    svg::Document svg_to_render;

private:
    const transport::TransportCatalogue& catalogue;
    renderer::MapBuilder& map_renderer; 
    
};  

} // exit interface    

