#pragma once

#include "geo.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "transport_router.h"

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
    const std::optional<json::Node> getRoutingSettings() const;
    
    //заполняем настройки маршрута:
    void SetRouter(transport::RouteSettings& router);
    
    //для заполнения каталога
    void FillCatalogue(transport::TransportCatalogue& catalogue);
    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStops(const json::Dict& dictionary) const;
    std::tuple<std::string_view, std::vector<std::string>, std::vector<std::shared_ptr<transport::Stop>>, transport::RouteType> FillBuses(const json::Dict& dictionary, transport::TransportCatalogue& catalogue) const;
    
    //для заполнения настроек рендеринга
    const renderer::RenderSettings SetRenderSettings() const;
    
    //обработка запросов и формирование общего ответа 
   json::Document ProcessReqs(const transport::TransportCatalogue& catalogue, const svg::Document& svg_to_render, const transport::RouteManager& route_manager) const ;

private:
    json::Document doc_;

};

/******************************************************************
                    фабрика обработки запросов
******************************************************************/
    
//абстрактный класс - запрос
class Query{
public:
    Query(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route)
        :dictionary(dict), 
        catalogue(cat),
        svg_to_render(svg), 
        route_manager(route)
        {}
    virtual const json::Node ProcessQuery() const = 0;
    virtual ~Query() = default;
    
protected:
    const json::Dict& dictionary;
    const transport::TransportCatalogue& catalogue;
    const svg::Document& svg_to_render;
    const transport::RouteManager& route_manager;    
};

//абстрактный класс-фабрика для обработка запроса
class QueryFactory {
public:
    virtual std::unique_ptr<Query> Construct(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route) const = 0;
    virtual ~QueryFactory() = default;

    static const QueryFactory& GetFactory(std::string_view id);
};

//класс для обработки запроса по остановкам
class StopQuery: public Query{
public:   
 using Query::Query;   
const json::Node ProcessQuery() const override ;
    class Factory : public QueryFactory {
    public:
    std::unique_ptr<Query> Construct(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route) const override {
        return std::make_unique<StopQuery>(dict, cat, svg, route);
        }
    };
};

//класс для обработки запроса по автобусам
class BusQuery: public Query{
public:   
using Query::Query;    
const json::Node ProcessQuery() const override ;
    class Factory : public QueryFactory {
    public:
    std::unique_ptr<Query> Construct(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route) const override {
        return std::make_unique<BusQuery>(dict, cat, svg, route);
        }
    };
};

//класс для добавление в запрос карты
class MapQuery: public Query{
public:
using Query::Query;    
const json::Node ProcessQuery() const override ;
    class Factory : public QueryFactory {
    public:
    std::unique_ptr<Query> Construct(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route) const override {
        return std::make_unique<MapQuery>(dict, cat, svg, route);
        }
    };
};

//класс для расчета оптимального пути и добавление в ответ
class RouteQuery: public Query{
public:
using Query::Query;    
const json::Node ProcessQuery() const override;

    class Factory : public QueryFactory {
    public:
    std::unique_ptr<Query> Construct(const json::Dict& dict,  const transport::TransportCatalogue& cat, const svg::Document& svg, const transport::RouteManager& route) const override {
        return std::make_unique<RouteQuery>(dict, cat, svg, route);
        }
    };
};
