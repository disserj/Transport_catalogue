#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"

#include "domain.h"
#include <iostream>
#include <unordered_set>



int main() {
    //читаем входящий поток:
    JsonManager manager(std::cin);
    
    //каталог
    transport::TransportCatalogue catalogue;
    
    //заполняем каталог менеджером:
    manager.FillCatalogue(catalogue);
    
    //инициализируем отрисовщик, передавая в конструктор прочитанные менеджером из потока настройки рендеринга:
    renderer::MapBuilder builder(manager.SetRenderSettings());
    
    //настройки маршрута:
    transport::RouteSettings route_settings;
    
    //заполняем настройки маршрута менеджером:
    manager.SetRouter(route_settings);
    
    //инициализируем RouteManager для расчета оптимального пути:
    transport::RouteManager router( route_settings, catalogue);
    
    //инициализируем хэндлер:
    interface::RequestHandler handler(catalogue, builder, router);
    
    //вывод документа в поток:
    handler.Response(std::cout, manager);
}
