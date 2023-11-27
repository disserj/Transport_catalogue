#include "request_handler.h"
#include <iostream>

namespace interface{
    

void RequestHandler::Response(std::ostream& out, JsonManager& manager){
   
    svg_to_render = map_renderer.BuildSvg(catalogue);
    
    json::Print(manager.ProcessReqs(catalogue, svg_to_render), out);
}    
    
    
}//exit interface


