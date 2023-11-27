#include "json_reader.h"

// возвращает узел с информацией для заполнения

const json::Node& JsonManager::getBaseRequests() const{
    if (!doc_.GetRoot().AsMap().count("base_requests")) {
        return nothing;
    }
    return doc_.GetRoot().AsMap().at("base_requests");
}

// возвращает узел - запрос 
const json::Node& JsonManager::getStatRequests() const{
    if (!doc_.GetRoot().AsMap().count("stat_requests"))  {
        return nothing;
    }
    return doc_.GetRoot().AsMap().at("stat_requests");
}

//возвращает узел с информацией для отображения на карте
const json::Node& JsonManager::getRenderSettings() const{
    if (!doc_.GetRoot().AsMap().count("render_settings"))  {
        return nothing;
    }
    return doc_.GetRoot().AsMap().at("render_settings");
}

//---------------------------------------------------------

// методы для заполнения каталога 
//основной:
void JsonManager::FillCatalogue(transport::TransportCatalogue& catalogue){
    //заполняем список остановок
    const json::Array& list_of_base_reqs = getBaseRequests().AsArray();
    for (auto& stops : list_of_base_reqs) {
        auto& stops_map = stops.AsMap();
        auto& type = stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop, crd, distances] = FillStops(stops_map);
            catalogue.AddStop((std::string)stop, crd);  
        }
    }
    //заполняем дистанции
    for (auto& stops : list_of_base_reqs)  {
        auto& stops_map = stops.AsMap();
        auto& type = stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop, crd, distances] = FillStops(stops_map);
            for (auto& [to_name, dist] : distances) {
                auto from = catalogue.FindStop((std::string)stop);
                auto to = catalogue.FindStop((std::string)to_name);
                catalogue.SetDistance(from, to, dist);
            }
        }
    }
    //заполняем список автобусов
    for (auto& buses : list_of_base_reqs) {
        const auto& bus_map = buses.AsMap();
        const auto& type = bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus, stops, cycle] = FillBuses(bus_map);
            catalogue.AddRoute((std::string)bus, stops, cycle);
        }
    }
}

// методы для заполнения каталога 
//вспомогательный:

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonManager::FillStops(const json::Dict& dictionary) const{
    std::string_view stop = dictionary.at("name").AsString();
    geo::Coordinates crd = { dictionary.at("latitude").AsDouble(), dictionary.at("longitude").AsDouble() };
    std::map<std::string_view, int> distances;
    auto& road_dists = dictionary.at("road_distances").AsMap();
    for (auto& [st, dist] : road_dists) {
        distances.emplace(st, dist.AsInt());
    }
    return {stop, crd, distances};
    
}

// методы для заполнения каталога 
//вспомогательный:

std::tuple<std::string_view, std::vector<std::string_view>, bool> JsonManager::FillBuses(const json::Dict& dictionary) const{
    std::string_view bus = dictionary.at("name").AsString();
    std::vector<std::string_view> stops;
    for (auto& stop : dictionary.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }
    bool cycle = dictionary.at("is_roundtrip").AsBool();

    return {bus, stops, cycle};
}

//-----------------------------------------------------------

//методы для формирования ответа
//основной:
json::Document JsonManager::ProcessReqs(const transport::TransportCatalogue& catalogue,const svg::Document& svg_to_render) const {
    const json::Array& list_of_stat_reqs = getStatRequests().AsArray();
    json::Array to_response_json_doc;
    for (auto& req : list_of_stat_reqs) {
        const auto& request_map = req.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop"){
           to_response_json_doc.push_back(ProcessStopRequest(request_map, catalogue).AsMap());
            
        }
        if (type == "Bus"){
            to_response_json_doc.push_back(ProcessRouteRequest(request_map, catalogue).AsMap());
        }
        if (type == "Map") to_response_json_doc.push_back(IncludeMapInResponse(request_map, svg_to_render).AsMap());
        }
    return json::Document{to_response_json_doc};
}


//методы для формирования ответа
//вспомогательный:
//включаем в ответ инф. о маршруте
const json::Node JsonManager::ProcessRouteRequest(const json::Dict& dictionary, const transport::TransportCatalogue& catalogue)const{
    json::Dict json_route_dict;
    const std::string& bus = dictionary.at("name").AsString();
    json_route_dict["request_id"] = dictionary.at("id").AsInt();

    if (!catalogue.FindRoute(bus)) {
        json_route_dict["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        int stops = 0;
        int unique = 0; 
        int len = 0;
        double curv = 0.0;
        std::tie(stops, unique, len, curv ) = catalogue.RouteInfo(bus);

        json_route_dict["curvature"] = curv;
        json_route_dict["route_length"] = len;
        json_route_dict["stop_count"] = stops;
        json_route_dict["unique_stop_count"] = unique;
    }

    return json::Node{json_route_dict};
}

//методы для формирования ответа
//вспомогательный:
//включаем в ответ инф. об остановках
const json::Node JsonManager::ProcessStopRequest(const json::Dict& dictionary, const transport::TransportCatalogue& catalogue) const{
    json::Dict json_stops_dict;
    const std::string& stop = dictionary.at("name").AsString();
    json_stops_dict["request_id"] = dictionary.at("id").AsInt();
    std::set<std::string>buses_list;
    buses_list = catalogue.GetBusesNums(stop);
    if (!catalogue.FindStop(stop)) {
        json_stops_dict["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for(const auto& bus: buses_list) {
            buses.push_back(bus);
        }
        json_stops_dict["buses"] = buses;
    }

    return json::Node{json_stops_dict};

}

//методы для формирования ответа
//вспомогательный:
//включаем в ответ карту
const json::Node JsonManager::IncludeMapInResponse(const json::Dict& request_map,const svg::Document& svg_to_render) const {
    json::Dict json_map_dict;
    json_map_dict["request_id"] = request_map.at("id").AsInt();
    std::stringstream ss;
    svg_to_render.Render(ss);
    json_map_dict["map"] = ss.str();

    return json::Node{json_map_dict};
}

//------------------------------------------------------
//для заполнения настроек рендеринга
const renderer::RenderSettings JsonManager::SetRenderSettings() const {
    const auto request_map = getRenderSettings().AsMap();
    renderer::RenderSettings render_settings;
    
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
    
    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        } else throw std::logic_error("wrong underlayer colortype");
    } else throw std::logic_error("wrong underlayer color");
    
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();
    
    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            } else throw std::logic_error("wrong color_palette type");
        } else throw std::logic_error("wrong color_palette");
    }
    
    
    return render_settings;
}
