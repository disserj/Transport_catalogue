#pragma once
#include "geo.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "json.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <sstream>


class JsonManager{
public:
    JsonManager(std::istream& in)
    : doc_(json::Load(in))
    {}
    //для работы с деревом документа
    const std::optional<json::Node> getBaseRequests() const;
    const std::optional<json::Node> getStatRequests() const;
    const std::optional<json::Node> getRenderSettings() const;
    
    //для заполнения каталога
    void FillCatalogue(transport::TransportCatalogue& catalogue);
    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStops(const json::Dict& dictionary) const;
    std::tuple<std::string_view, std::vector<std::string>, std::vector<std::shared_ptr<transport::Stop>>, bool> FillBuses(const json::Dict& dictionary, transport::TransportCatalogue& catalogue) const;
    
    //для заполнения настроек рендеринга
    const renderer::RenderSettings SetRenderSettings() const;
    
    //для формирования ответа
   json::Document ProcessReqs(const transport::TransportCatalogue& catalogue, const svg::Document& svg_to_render) const ;
    const json::Node ProcessRouteRequest(const json::Dict& dictionary, const transport::TransportCatalogue& catalogue) const;
    const json::Node ProcessStopRequest(const json::Dict& dictionary, const transport::TransportCatalogue& catalogue) const;
    const json::Node IncludeMapInResponse(const json::Dict& request_map,const svg::Document& svg_to_render) const;

    
    
private:
    json::Document doc_;


};
    
