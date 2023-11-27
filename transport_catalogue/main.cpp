#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "domain.h"
#include <iostream>


using namespace interface;
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
    
    //инициализируем интерфейс каталогом и отрисовщиком:
    RequestHandler handler(catalogue, builder);
    
    //вывод документа в поток, интерфейс использует для этого менеджер для обработки документа:
    handler.Response(std::cout, manager);
}
