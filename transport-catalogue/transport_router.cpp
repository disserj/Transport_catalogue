#include "transport_router.h"

namespace transport{
    
    bool operator<(const EdgeData &lhs, const EdgeData &rhs) {
        return lhs.total_time < rhs.total_time;
    }
    bool operator>(const EdgeData &lhs, const EdgeData &rhs) {
        return lhs.total_time > rhs.total_time;
    }
    EdgeData operator+(const EdgeData &lhs, const EdgeData &rhs) {
        EdgeData result;
        result.total_time = lhs.total_time + rhs.total_time;
        return result;
    }
    
	const std::optional<Router::RouteInfo> RouteManager::CalculateShortestRoute(std::string_view from, std::string_view to) const{
        return router->BuildRoute(stops_to_points.at(std::string(from)),stops_to_points.at(std::string(to)));
    }
	const std::shared_ptr<Graph>& RouteManager::GetGraph() const{
        return graph;
    }
    
    
}//exit transport