#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "domain.h"
#include <iostream>

using namespace transport;
using namespace renderer;

int main() {
    //читаем входящий поток:
    JsonManager manager(std::cin);
    
    //каталог
    transport::TransportCatalogue catalogue;
    
    //заполняем каталог менеджером:
    manager.FillCatalogue(catalogue);
    
    //инициализируем отрисовщик, передавая в конструктор прочитанные менеджером из потока настройки рендеринга:
    MapBuilder builder(manager.SetRenderSettings());

    //вывод документа в поток:
    interface::Response(std::cout, manager, builder, catalogue);
}
