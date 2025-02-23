#pragma once

#include "geo.h"
#include "domain.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <optional>

namespace transport_catalogue {
    class TransportCatalogue {
    public:
        void AddStop(std::string_view title, geo::Coordinates coords);

        void SetStopsDistance(std::string_view from, std::string_view to, int distance);

        void AddBus(std::string_view title, const std::vector<std::string_view>& stops, bool is_roundtrip);

        int GetDistance(const std::string& from, const std::string& to) const;

        const Bus* GetBus(std::string_view title) const;
        std::optional<BusStats> GetBusStats(std::string_view title) const;
        const std::unordered_set<const Bus*>& GetBusesOfStop(std::string_view title) const;
        const Stop* GetStop(std::string_view title) const;

        const std::deque<Stop>& GetStops();
        const std::deque<Bus>& GetBuses();
    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;

        std::unordered_map<std::string_view, const Stop*> stops_index_;
        std::unordered_map<std::string_view, const Bus*> buses_index_;

        std::unordered_map<std::string_view, std::unordered_set<const Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<std::string, std::string>, int, PairHash> stops_to_distance_;
    };
}



