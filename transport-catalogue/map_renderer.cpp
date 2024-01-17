#include "map_renderer.h"

namespace renderer {

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

const RenderSettings& MapBuilder::getSettings() const {
    return settings;
}

//возвращает готовый svg документ
svg::Document MapBuilder::BuildSvg([[maybe_unused]]const transport::TransportCatalogue& catalogue){
    svg::Document svg_doc;
    const std::map<std::string_view, std::shared_ptr<transport::Bus>> list_buses = catalogue.GetBusCatalogue();
    std::map<std::string_view, std::shared_ptr<transport::Stop>> list_stops;
    std::vector<geo::Coordinates> stops_crd;
    
    for (const auto& [bus_number, bus] : list_buses) {
        
        if (bus.get()->GetStopsPtrs().empty()) continue;
        for (const auto& stop : bus.get()->GetStopsPtrs()) {
            
            stops_crd.push_back(stop.get()->point);
            list_stops[stop.get()->name] = stop;
            
        }
    }

    SphereProjector sp(stops_crd.begin(), stops_crd.end(), settings.width, settings.height, settings.padding);
    
    for (auto& line : RouteOutline(catalogue, sp)) svg_doc.Add(line);
    for (const auto& text : RenderBusLabel(catalogue, sp)) svg_doc.Add(text);
    for (const auto& circle : RenderStopsCircles(list_stops, sp)) svg_doc.Add(circle);
     for (const auto& text : RenderStopsNames(list_stops, sp)) svg_doc.Add(text);
    
    return svg_doc;
}
    
//отображение маршрутов в виде множества ломаных 
std::vector<svg::Polyline> MapBuilder::RouteOutline(const transport::TransportCatalogue& catalogue,[[maybe_unused]] const SphereProjector& sp) const{
   [[maybe_unused]] const std::map<std::string_view, std::shared_ptr<transport::Bus>> list_buses = catalogue.GetBusCatalogue();
    std::vector<svg::Polyline> polyline_container;
    
   [[maybe_unused]] size_t color_num = 0;
    for (const auto& [bus_number, bus] : list_buses) {
        if (bus.get()->GetStopsPtrs().empty()) continue;
         std::vector<std::shared_ptr<transport::Stop>> route_stops{bus.get()->stop_ptrs.begin(),bus.get()->stop_ptrs.end()};

        if (bus.get()->type == transport::RouteType::LINEAR ) {
              
               for(size_t i= route_stops.size()-2;(int)i>=0;--i){
                   route_stops.push_back(route_stops[i]);
               }
       
            }
 
        
        svg::Polyline line;
    std::for_each(route_stops.begin(),route_stops.end(),[&line,&sp](std::shared_ptr<transport::Stop> stop){
            line.AddPoint(sp(stop.get()->point));
        });
      
        
    
            line.SetStrokeColor(settings.color_palette[color_num]);
        line.SetFillColor("none");
        line.SetStrokeWidth(settings.line_width);
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        if (color_num < (settings.color_palette.size() - 1)) ++color_num;
        else color_num = 0;
        
        polyline_container.push_back(line);
    }
    
    return polyline_container;
    
}

//отображение номеров маршрутов
std::vector<svg::Text> MapBuilder::RenderBusLabel(const transport::TransportCatalogue& catalogue, const SphereProjector& sp) const {
    const std::map<std::string_view, std::shared_ptr<transport::Bus>> list_buses = catalogue.GetBusCatalogue();
    std::vector<svg::Text> bus_labels;
    size_t color_num = 0;
    for (const auto& [bus_number, bus] :list_buses) {
        std::vector< std::shared_ptr<transport::Stop>> route_stops_ptrs = bus.get()->GetStopsPtrs();
        if (bus.get()->stops.empty()) continue;
        svg::Text text;
        svg::Text underlayer;
        text.SetPosition(sp(route_stops_ptrs[0].get()->point));
        text.SetOffset(settings.bus_label_offset);
        text.SetFontSize(settings.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(bus.get()->num);
        text.SetFillColor(settings.color_palette[color_num]);
        if (color_num < (settings.color_palette.size() - 1)) ++color_num;
        else color_num = 0;
        
        underlayer.SetPosition(sp(route_stops_ptrs[0].get()->point));
        underlayer.SetOffset(settings.bus_label_offset);
        underlayer.SetFontSize(settings.bus_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetFontWeight("bold");
        underlayer.SetData(bus.get()->num);
        underlayer.SetFillColor(settings.underlayer_color);
        underlayer.SetStrokeColor(settings.underlayer_color);
        underlayer.SetStrokeWidth(settings.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        bus_labels.push_back(underlayer);
        bus_labels.push_back(text);
        
        if (bus.get()->type == transport::RouteType::LINEAR && route_stops_ptrs[0].get() != route_stops_ptrs[route_stops_ptrs.size() - 1].get()) {
            svg::Text text2 {text};
            svg::Text underlayer2 {underlayer};
            text2.SetPosition(sp(route_stops_ptrs[route_stops_ptrs.size() - 1].get()->point));
            underlayer2.SetPosition(sp(route_stops_ptrs[route_stops_ptrs.size() - 1].get()->point));
            
            bus_labels.push_back(underlayer2);
            bus_labels.push_back(text2);
        }
    }
    
    return bus_labels;
}

//отображение точек остановок в виде множества кругов
std::vector<svg::Circle> MapBuilder::RenderStopsCircles(const std::map<std::string_view, std::shared_ptr<transport::Stop>>& list_stops, const SphereProjector& sp) const {

    std::vector<svg::Circle> circle_container;
  
        for (const auto& [name,stop] : list_stops) {

            svg::Circle circle;
            circle.SetCenter(sp(stop.get()->point));
            circle.SetRadius(settings.stop_radius);
            circle.SetFillColor("white");
            
            circle_container.push_back(circle);
        }
    
    
    return circle_container;
}




//отображение названий остановок
std::vector<svg::Text> MapBuilder::RenderStopsNames(const std::map<std::string_view, std::shared_ptr<transport::Stop>>& list_stops, const SphereProjector& sp) const {
    std::vector<svg::Text> names;
    svg::Text text;
    svg::Text underlayer;
 
    for (const auto& [stop_name, stop] : list_stops) {

        text.SetPosition(sp(stop.get()->point));
        text.SetOffset(settings.stop_label_offset);
        text.SetFontSize(settings.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData((std::string)stop_name);
        text.SetFillColor("black");
        
        underlayer.SetPosition(sp(stop.get()->point));
        underlayer.SetOffset(settings.stop_label_offset);
        underlayer.SetFontSize(settings.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData((std::string)stop_name);
        underlayer.SetFillColor(settings.underlayer_color);
        underlayer.SetStrokeColor(settings.underlayer_color);
        underlayer.SetStrokeWidth(settings.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        names.push_back(underlayer);
        names.push_back(text);
             
    }
    
    return names;
}

}//exit renderer 

    