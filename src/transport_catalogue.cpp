#include "transport_catalogue.h"
#include "geo.h"
#include <optional>
#include <unordered_set>

namespace transport_catalogue {
    void TransportCatalogue::AddStop(std::string_view title, geo::Coordinates coords) {
        stops_.insert({std::string(title), {std::string(title), coords, {}}});
    }

    void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops) {
        std::optional<geo::Coordinates> prev_coords;
        std::vector<Stop const *> stops_pointers;

        std::unordered_set<std::string_view> uniq_stops;
        uniq_stops.reserve(stops.size());

        Bus& bus = buses_.insert({std::string(title), Bus{}}).first->second;
        bus.title = title;

        for (const std::string_view stop_title : stops) {
            Stop& stop = stops_.at(std::string(stop_title));

            bus.stops.push_back(&stop);
            stop.buses.insert(&bus);

            uniq_stops.insert(stop_title);

            if(prev_coords.has_value()) {
                bus.route_length += ComputeDistance(prev_coords.value(), stop.coords);
            }

            prev_coords = stop.coords;
        }

        bus.stops_amount = bus.stops.size();
        bus.uniq_stops_amount = uniq_stops.size();
    }

    const Bus& TransportCatalogue::GetBus(std::string_view title) const {
        return buses_.at(std::string{title});
    }

    const Stop &TransportCatalogue::GetStop(std::string_view title) const {
        return stops_.at(std::string{title});
    }

    bool TransportCatalogue::ContainsBus(std::string_view title) const {
        return buses_.contains(std::string(title));
    }

    bool TransportCatalogue::ContainsStop(std::string_view title) const {
        return stops_.contains(std::string(title));
    }
}
