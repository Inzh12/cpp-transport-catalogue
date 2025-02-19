#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <iostream>

using namespace transport_catalogue;

int main() {
    MapRenderer renderer;
    TransportCatalogue catalogue;
    RequestHandler request_hander(catalogue, renderer);
    JsonReader reader(catalogue, request_hander, renderer, std::cin);

    reader.FillCatalogue();
    reader.PrintStats(std::cout);
}
