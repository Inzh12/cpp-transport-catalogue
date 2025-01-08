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

    void TransportCatalogue::SetNearbyStopsDistances(std::string_view title,
                                                     std::map<std::string, int> &stop_to_dist) {

        //const auto& current_stop = stops_index_.at(title);
        for (const auto& [nearby_stop_title, dist] : stop_to_dist) {
            //const auto& nearby_stop = stops_index_.at(nearby_stop_title);
            stops_to_distance_.insert({{std::string{title}, nearby_stop_title}, dist});
            stops_to_distance_[{nearby_stop_title, std::string{title}}] = dist;
        }

        // auto& current_stop_distances = stop_to_distances_[title];
        // for (const auto& [stop, dist] : stop_to_dist) {
        //     stop_to_distances_[title].insert({stop, dist});
        //     current_stop_distances[next_stop] = dist;
        // }
    }

    void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops) {
        std::unordered_set<std::string_view> uniq_stops;
        
        uniq_stops.reserve(stops.size());

        Bus& bus = *buses_.insert(buses_.end(), Bus{});
        BusStats& stats = bus_to_stats_.insert({&bus, BusStats{}}).first->second;

        bus.title = title;
        buses_index_.insert({bus.title, &bus});

        double geo_length = 0;
        const Stop* prev_stop = stops_index_.at(std::string(stops.front()));
        bus.stops.push_back(prev_stop);
        uniq_stops.insert(prev_stop->title);

        for (const std::string_view stop_title : std::span(stops.begin() + 1, stops.end())) {
            const Stop& stop = *stops_index_.at(std::string(stop_title));

            bus.stops.push_back(&stop);
            stop_to_buses_[stop.title].insert(&bus);
            uniq_stops.insert(stop_title);

            geo_length += ComputeDistance(prev_stop->coords, stop.coords);

            stats.route_length += stops_to_distance_.at({prev_stop->title, stop.title});

            // stats.route_length += stop_to_distances_.at(prev_stop->title)
            //                          .at(stop->title);

            prev_stop = &stop;
        }

        stats.curvature = stats.route_length / geo_length;
        stats.stops_amount = bus.stops.size();
        stats.uniq_stops_amount = uniq_stops.size();
    }

    const Bus* TransportCatalogue::GetBus(std::string_view title) const {
        if (!buses_index_.contains(std::string{title})) {
            return nullptr;
        }

        return buses_index_.at(std::string{title});
    }

    const BusStats* TransportCatalogue::GetBusStats(std::string_view title) const {
        const Bus* bus = GetBus(title);

        if (!bus_to_stats_.contains(bus)) {
            return nullptr;
        }

        return &bus_to_stats_.at(bus);
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
