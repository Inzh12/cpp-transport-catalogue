#include "transport_catalogue.h"
#include "geo.h"

#include <unordered_set>
#include <span>

namespace transport_catalogue {
    void TransportCatalogue::AddStop(std::string_view title, geo::Coordinates coords) {
        const Stop* stop = &*stops_.insert(stops_.end(), {std::string(title), coords});
        stops_index_.insert({stop->title, stop});
        stop_to_buses_.insert({stop->title, {}});
    }

    void TransportCatalogue::SetStopsDistance(std::string_view stop1,
                                              std::string_view stop2,
                                              int distance) {

        stops_to_distance_.insert({{std::string(stop1), std::string(stop2)}, distance});
        stops_to_distance_[{std::string(stop2), std::string(stop1)}] = distance;
    }

    void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops) {
        Bus& bus = *buses_.insert(buses_.end(), Bus{});

        bus.title = title;
        buses_index_.insert({bus.title, &bus});

        for (std::string_view stop_title : stops) {
            const Stop& stop = *stops_index_.at(std::string(stop_title));
            bus.stops.push_back(&stop);
            stop_to_buses_[stop.title].insert(&bus);
        }
    }

    const Bus* TransportCatalogue::GetBus(std::string_view title) const {
        if (!buses_index_.contains(std::string{title})) {
            return nullptr;
        }

        return buses_index_.at(std::string{title});
    }

    BusStats TransportCatalogue::GetBusStats(std::string_view title) const {
        if (!buses_index_.contains(title)) {
            return BusStats {};
        }

        const Bus* bus = buses_index_.at(title);
        std::vector<const Stop*> stops = bus->stops;
        const Stop* prev_stop = stops.front();
        double geo_length = 0;
        std::unordered_set<std::string_view> uniq_stops;

        uniq_stops.reserve(stops.size());
        uniq_stops.insert(prev_stop->title);

        BusStats stats;

        for (const Stop* stop : std::span(stops.begin() + 1, stops.end())) {
            uniq_stops.insert(stop->title);
            geo_length += ComputeDistance(prev_stop->coords, stop->coords);
            stats.route_length += stops_to_distance_.at({prev_stop->title, stop->title});

            prev_stop = stop;
        }

        stats.curvature = stats.route_length / geo_length;
        stats.stops_amount = bus->stops.size();
        stats.uniq_stops_amount = uniq_stops.size();

        return stats;
    }

    const std::unordered_set<const Bus*>&
    TransportCatalogue::GetBusesOfStop(std::string_view title) const {
        return stop_to_buses_.at(title);
    }

    const Stop* TransportCatalogue::GetStop(std::string_view title) const {
        if (!stops_index_.contains(std::string{title})) {
            return nullptr;
        }

        return stops_index_.at(std::string{title});
    }
}
