#pragma once
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include <utility>
#include <unordered_map>
#include <string_view>

namespace transport{
    
constexpr static double KM_H = 100.0 / 6.0;
struct EdgeData;   
    
using Graph = graph::DirectedWeightedGraph<EdgeData>;
using Router = graph::Router<EdgeData>;    
    
struct RouteSettings{
   int bus_wait_time = 0;
   double bus_velocity = 0;
};    
    
struct EdgeData{
    std::string_view name;
    int span_count = 0;
    double total_time = 0; 
};   

bool operator < (const EdgeData &lhs, const EdgeData &rhs) ;
bool operator > (const EdgeData &lhs, const EdgeData &rhs) ;
EdgeData operator + (const EdgeData &lhs, const EdgeData &rhs) ; 


class RouteManager{
public: 
    template <typename TransportCatalogue>
    RouteManager(const RouteSettings& route_settings, TransportCatalogue&& catalogue) {
        wait_time = route_settings.bus_wait_time;
        velocity = route_settings.bus_velocity;
        // записываем маршруты в граф
        BuildGraph(catalogue);
        // строим маршрутизатор
        router = std::make_unique<Router>(*graph.get());
	}
    template <typename TransportCatalogue>
    void BuildGraph(TransportCatalogue&& catalogue);
    
	const std::optional<Router::RouteInfo> CalculateShortestRoute(std::string_view from, std::string_view to) const;
	const std::shared_ptr<Graph>& GetGraph() const;
        
private:
    double wait_time = 0;
    double velocity = 0;
    //указатель на созданный граф
    std::shared_ptr<Graph> graph;
    //таблица "имя остановки - вершина графа"
    std::unordered_map<std::string_view, graph::VertexId> stops_to_points;
    //указатель на маршрутизатор, созданный на графе
	std::unique_ptr<Router> router;
};

//строим граф
template <typename TransportCatalogue>    
void RouteManager::BuildGraph(TransportCatalogue&& catalogue) {
    const auto& stop_catalogue = std::forward<TransportCatalogue>(catalogue).GetStopCatalogue();
    const auto& bus_catalogue = std::forward<TransportCatalogue>(catalogue).GetBusCatalogue();
    
    Graph stops_graph(stop_catalogue.size() * 2);
    std::unordered_map<std::string_view, graph::VertexId> stop_ids;
    graph::VertexId vertex_id = 0;
    
    std::for_each(stop_catalogue.begin(),stop_catalogue.end(),
    [this, &stop_ids, &stops_graph, &vertex_id](const auto& name_and_data){
    stop_ids[name_and_data.second.get()->name] = vertex_id;
    stops_graph.AddEdge({
                vertex_id,
                ++vertex_id,
                EdgeData({name_and_data.second.get()->name, 0, wait_time})
            });
        ++vertex_id;
    });
    stops_to_points = std::move(stop_ids);

    std::for_each(bus_catalogue.begin(), bus_catalogue.end(),
        [this, &stops_graph, &catalogue](const auto& num_and_data) {
            const auto& bus_data = num_and_data.second;
            const auto& stops = bus_data.get()->stop_ptrs;
            size_t stops_count = stops.size();
            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                
                    const std::shared_ptr<Stop> from = stops[i];
                    const std::shared_ptr<Stop> to = stops[j];
                    
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;
                    
                    for (size_t x = i + 1; x <= j; ++x) {
                        dist_sum += catalogue.GetDistance(stops[x - 1].get(), stops[x].get());
                        dist_sum_inverse += catalogue.GetDistance(stops[x].get(), stops[x - 1].get());
                      
                    }
                    
                    stops_graph.AddEdge({
                    stops_to_points.at(from.get()->name) + 1,
                    stops_to_points.at(to.get()->name),
                    
                    EdgeData({bus_data.get()->num, (int)(j-i), (double)(dist_sum) / (velocity * KM_H)})
                    });    
                    if (bus_data.get()->type == RouteType::LINEAR) {
                        stops_graph.AddEdge({
                        stops_to_points.at(to.get()->name) + 1,
                        stops_to_points.at(from.get()->name),
                        
                        EdgeData({bus_data.get()->num, (int)(j-i), (double)(dist_sum_inverse) / (velocity * KM_H)})
                        });
                    }
                }
            }
        });

    graph = std::make_shared<Graph>(stops_graph);
}
    
}//exit transport