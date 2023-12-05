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

    //инициализируем хэндлер:
    interface::RequestHandler handler(catalogue, builder);
    
    //вывод документа в поток:
    handler.Response(std::cout, manager);
}
