#pragma once
#include "transport_catalogue.h"
#include "input_reader.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

namespace transport{

void Response(std::ostream& out,Request_Queue& request, TransportCatalogue& catalogue);

void BusResponse(std::ostream& out, std::pair<std::string,bool> bus,TransportCatalogue& catalogue);

void StopResponse(std::ostream& out, std::pair<std::string,bool> stop,TransportCatalogue& catalogue);
    
} // exit transport    
