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
    //void SetRequestId(json::DictRef dict_ref, int id);
    void AddStopStats(json::DictRef stat, const std::string& stop_name);
    void AddBusStats(json::DictRef stat, const std::string& bus_name);
    void AddMap(json::DictRef stat);
    //json::Dict MakeStopStat(const json::Dict& stat_request);
    //json::Dict MakeBusStat(const json::Dict& stat_request);
    //json::Dict MakeMapStat(const json::Dict& stat_request);

    svg::Color ReadColor(const json::Node& color_node);

    TransportCatalogue& catalogue_;
    RequestHandler& request_hander_;
    MapRenderer& renderer_;
    json::Dict root_;
};

}
