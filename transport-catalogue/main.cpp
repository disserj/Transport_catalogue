#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>

using namespace transport;

int main() {
    TransportCatalogue catalogue;
    Request_Queue request;
    Read(std::cin, request);
    FillCatalogue(request, catalogue);
    Response(std::cout, request, catalogue);
}
