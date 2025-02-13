#pragma once

#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json.h"

namespace transport_catalogue {

class JsonReader {
public:
    JsonReader(TransportCatalogue& catalogue,
               RequestHandler& request_hander,
               MapRenderer& renderer,
               std::istream &input);

    void FillCatalogue();
    void PrintStats(std::ostream& output);
    void SetRenderSettings();
private:
    svg::Color ReadColor(const json::Node& color_node);

    TransportCatalogue& catalogue_;
    RequestHandler& request_hander_;
    MapRenderer& renderer_;
    json::Dict root_;
};

}
