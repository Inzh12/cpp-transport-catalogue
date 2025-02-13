#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue {

class RequestHandler {
public:
    RequestHandler(TransportCatalogue& db, const MapRenderer& renderer);

    void AddBus(std::string_view title, const std::vector<std::string_view> &stops, bool is_roundtrip);
    std::string RenderMap() const;
private:
    TransportCatalogue& db_;
    const MapRenderer& renderer_;
};

}


