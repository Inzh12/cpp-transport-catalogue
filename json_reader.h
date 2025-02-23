#pragma once

#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

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
    void FillStops(const json::Array& base_requests);
    void FillBuses(const json::Array& base_requests);
    void AddStopStats(json::Builder::DictRef stat, const std::string& stop_name);
    void AddBusStats(json::Builder::DictRef stat, const std::string& bus_name);
    void AddMap(json::Builder::DictRef stat);

    svg::Color ReadColor(const json::Node& color_node);

    TransportCatalogue& catalogue_;
    RequestHandler& request_hander_;
    MapRenderer& renderer_;
    json::Dict root_;
};

}
