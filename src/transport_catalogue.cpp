#include "transport_catalogue.h"
#include "geo.h"
#include <optional>
#include <unordered_set>

namespace transport_catalogue {
    void TransportCatalogue::AddStop(std::string_view title, geo::Coordinates coords) {
        const Stop* stop = &*stops_.insert(stops_.end(), {std::string(title), coords});
        stops_index_.insert({std::string(title), stop});
    }

    void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops) {
        std::optional<geo::Coordinates> prev_coords;
        std::unordered_set<std::string_view> uniq_stops;
        
        uniq_stops.reserve(stops.size());

        Bus* bus = &*buses_.insert(buses_.end(), Bus{});
        buses_index_.insert({std::string(title), bus});

        bus->title = title;

        for (const std::string_view stop_title : stops) {
            auto stop_it = stops_index_.at(std::string(stop_title));

            bus->stops.push_back(stop_it);
            stop_to_buses_[stop_it->title].insert(bus);
            uniq_stops.insert(stop_title);

            if(prev_coords.has_value()) {
                bus->route_length += ComputeDistance(prev_coords.value(), stop_it->coords);
            }

            prev_coords = stop_it->coords;
        }

        bus->stops_amount = bus->stops.size();
        bus->uniq_stops_amount = uniq_stops.size();
    }

    const Bus* TransportCatalogue::GetBus(std::string_view title) const {
        if (!buses_index_.contains(std::string{title})) {
            return nullptr;
        }

        return buses_index_.at(std::string{title});
    }

    const std::set<const Bus*, BusPoinerComapare>* TransportCatalogue::GetBusesOfStop(std::string_view title) const
    {
        if (!stop_to_buses_.contains(std::string{title})) {
            return nullptr;
        }

        return &stop_to_buses_.at(std::string(title));
    }

    const Stop* TransportCatalogue::GetStop(std::string_view title) const {
        if (!stops_index_.contains(std::string{title})) {
            return nullptr;
        }

        return stops_index_.at(std::string{title});
    }
}
