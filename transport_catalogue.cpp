#include "transport_catalogue.h"
#include "geo.h"

#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace transport_catalogue {

void TransportCatalogue::AddStop(std::string_view title, geo::Coordinates coords) {
    const Stop* stop = &*stops_.insert(stops_.end(), {std::string(title), coords});
    stops_index_.insert({stop->title, stop});
    stop_to_buses_.insert({stop->title, {}});
}

void TransportCatalogue::SetStopsDistance(std::string_view from,
                                          std::string_view to,
                                          int distance) {

    stops_to_distance_[{std::string(from), std::string(to)}] = distance;
}

void TransportCatalogue::AddBus(std::string_view title, const std::vector<std::string_view> &stops, bool is_roundtrip) {
    Bus& bus = *buses_.insert(buses_.end(), Bus{});

    bus.title = title;
    bus.is_roundtrip = is_roundtrip;
    buses_index_.insert({bus.title, &bus});

    for (std::string_view stop_title : stops) {
        const Stop& stop = *stops_index_.at(std::string(stop_title));
        bus.stops.push_back(&stop);
        stop_to_buses_[stop.title].insert(&bus);
    }
}

const Bus* TransportCatalogue::GetBus(std::string_view title) const {
    auto bus_it = buses_index_.find(title);

    if (bus_it != buses_index_.end()) {
        return nullptr;
    }

    return bus_it->second;
}

std::optional<BusStats> TransportCatalogue::GetBusStats(std::string_view title) const {
    auto bus_it = buses_index_.find(title);

    if (bus_it != buses_index_.end()) {
        return  std::nullopt;
    }

    const Bus* bus = bus_it->second;
    std::vector<const Stop*> stops = bus->stops;

    if(stops.size() == 1) {
        return BusStats{1, 1, 0, std::nan("")};
    } else if (stops.size() == 0) {
        return {};
    }

    double geo_length = 0;

    std::unordered_set<std::string_view> uniq_stops;
    uniq_stops.reserve(stops.size());

    BusStats stats;

    for (int i = 0; i < static_cast<int>(stops.size()) - 1; ++i) {
        uniq_stops.insert(stops[i]->title);

        geo_length += ComputeDistance(stops[i]->coords, stops[i+1]->coords);
        stats.route_length += GetDistance(stops[i]->title, stops[i+1]->title);
    }

    stats.stops_amount = bus->stops.size();

    stats.uniq_stops_amount = uniq_stops.size();
    stats.curvature = stats.route_length / geo_length;

    return stats;
}

const std::unordered_set<const Bus*>&
TransportCatalogue::GetBusesOfStop(std::string_view title) const {
    return stop_to_buses_.at(title);
}

const Stop* TransportCatalogue::GetStop(std::string_view title) const {
    auto stop_it = stops_index_.find(title);

    if (stop_it == stops_index_.end()) {
        return nullptr;
    }

    return stop_it->second;
}

const std::deque<Stop>& TransportCatalogue::GetStops() {
    return stops_;
}

const std::deque<Bus>& TransportCatalogue::GetBuses() {
    return buses_;
}

int TransportCatalogue::GetDistance(const std::string& from, const std::string& to) const
{
    if (stops_to_distance_.contains({from, to})) {
        return stops_to_distance_.at({from, to});
    } else if (from == to) {
        return 0;
    } else if (stops_to_distance_.contains({to, from})) {
        return stops_to_distance_.at({to, from});
    } else {
        throw std::out_of_range{"Can't find distance"};
    }
}

}
