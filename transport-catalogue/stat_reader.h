#pragma once
#include "transport_catalogue.h"
#include "input_reader.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

namespace transport{

void Response(std::ostream& out, RequestQueue& request, const TransportCatalogue& catalogue);

void BusResponse(std::ostream& out, const std::pair<std::string,bool>& bus, const TransportCatalogue& catalogue);

void StopResponse(std::ostream& out, const std::pair<std::string,bool>& stop, const TransportCatalogue& catalogue);
    
} // exit transport    
