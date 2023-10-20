#pragma once
#include "geo.h"
#include "transport_catalogue.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

namespace transport{

struct RequestQueue{
    std::vector<std::string> ToFill;
    std::vector<std::string> ToResponse;
};


void Read(std::istream& in, RequestQueue& request);
void FillCatalogue(RequestQueue& request, TransportCatalogue& catalogue);
std::pair<std::vector<std::string>, bool> ReadRoute(std::string& str);

std::pair<std::string,std::string> FillStops(std::string& str, TransportCatalogue& catalogue);
void FillBuses(std::string& str, TransportCatalogue& catalogue);
void FillDistances(std::string& name, std::string& str, TransportCatalogue& catalogue);
  
}// exit transport    
