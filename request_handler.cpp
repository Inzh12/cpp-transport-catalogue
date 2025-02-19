#include "request_handler.h"

#include <algorithm>
#include <ostream>

namespace transport_catalogue {

RequestHandler::RequestHandler(TransportCatalogue &db, const MapRenderer &renderer)
    : db_(db), renderer_(renderer)
{}

void RequestHandler::AddBus(std::string_view title, const std::vector<std::string_view> &stops, bool is_roundtrip) {
    if(!is_roundtrip) {
        std::vector<std::string_view> expanded_stops(stops);

        for (auto stop_it = stops.rbegin() + 1; stop_it != stops.rend(); ++stop_it) {
            expanded_stops.push_back(*stop_it);
        }

        db_.AddBus(title, expanded_stops, is_roundtrip);
        return;
    }

    db_.AddBus(title, stops, is_roundtrip);
}

std::string RequestHandler::RenderMap() const {

    const std::deque<Bus>& buses = db_.GetBuses();
    std::vector<const Bus*> sorted_buses;
    sorted_buses.reserve(buses.size());
    for (const Bus& bus : buses) {
        sorted_buses.push_back(&bus);
    }

    std::sort(sorted_buses.begin(), sorted_buses.end(), [](const Bus* lhs, const Bus* rhs){
        return lhs->title < rhs->title;
    });


    const std::deque<Stop>& stops = db_.GetStops();
    std::vector<const Stop*> sorted_stops_with_buses;
    for (const Stop& stop : stops) {
        if(!db_.GetBusesOfStop(stop.title).empty()) {
            sorted_stops_with_buses.push_back(&stop);
        }
    }

    std::sort(sorted_stops_with_buses.begin(), sorted_stops_with_buses.end(), [](const Stop* lhs, const Stop* rhs) {
        return lhs->title < rhs->title;
    });

    return renderer_.Render(sorted_buses, sorted_stops_with_buses);
}

}

