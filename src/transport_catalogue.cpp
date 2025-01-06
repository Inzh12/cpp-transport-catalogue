#include "transport_catalogue.h"
#include "geo.h"

#include <unordered_set>
#include <span>

namespace transport_catalogue {
    void TransportCatalogue::AddStop(std::string_view title, geo::Coordinates coords,
                                 std::vector<std::pair<int, std::string>>& dists_with_stops) {

        const Stop* stop = &*stops_.insert(stops_.end(), {std::string(title), coords});
        stops_index_.insert({stop->title, stop});
        stop_to_buses_.insert({stop->title, {}});

        auto& current_stop_distances = stop_to_distances_[stop->title];
        for (const auto& [dist, next_stop] : dists_with_stops) {
            stop_to_distances_[next_stop].insert({stop->title, dist});
            current_stop_distances[next_stop] = dist;
        }
    }

    void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops) {
        std::unordered_set<std::string_view> uniq_stops;
        
        uniq_stops.reserve(stops.size());

        Bus* bus = &*buses_.insert(buses_.end(), Bus{});
        bus->title = title;
        buses_index_.insert({bus->title, bus});

        double geo_length = 0;
        auto prev_stop = stops_index_.at(std::string(stops.front()));
        bus->stops.push_back(prev_stop);
        uniq_stops.insert(prev_stop->title);

        for (const std::string_view stop_title : std::span(stops.begin() + 1, stops.end())) {
            auto stop = stops_index_.at(std::string(stop_title));

            bus->stops.push_back(stop);
            stop_to_buses_[stop->title].insert(bus);
            uniq_stops.insert(stop_title);

            geo_length += ComputeDistance(prev_stop->coords, stop->coords);
            bus->route_length += stop_to_distances_.at(prev_stop->title)
                                     .at(stop->title);

            prev_stop = stop;
        }

        bus->curvature = bus->route_length / geo_length;
        bus->stops_amount = bus->stops.size();
        bus->uniq_stops_amount = uniq_stops.size();
    }

    const Bus* TransportCatalogue::GetBus(std::string_view title) const {
        if (!buses_index_.contains(std::string{title})) {
            return nullptr;
        }

        return buses_index_.at(std::string{title});
    }

    const std::set<const Bus*, BusPoinerComapare>& TransportCatalogue::GetBusesOfStop(std::string_view title) const
    {
        return stop_to_buses_.at(std::string(title));
    }

    const Stop* TransportCatalogue::GetStop(std::string_view title) const {
        if (!stops_index_.contains(std::string{title})) {
            return nullptr;
        }

        return stops_index_.at(std::string{title});
    }
}
