#pragma once

#include "geo.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace transport_catalogue {

    struct Stop {
        std::string title;
        geo::Coordinates coords;
    };

    struct Bus {
        std::string title;
        std::vector<const Stop*> stops;
    };

    struct BusStats {
        int stops_amount = 0;
        int uniq_stops_amount = 0;
        int route_length = 0;
        double curvature = 0;
    };

    struct PairHash {
    public:
        template <typename T, typename U>
        std::size_t operator()(const std::pair<T, U> &x) const
        {
            return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
        }
    };

    class TransportCatalogue {
    public:
        void AddStop(std::string_view title, geo::Coordinates coords);

        void SetStopsDistance(std::string_view stop1, std::string_view stop2, int distance);

        void AddBus(std::string_view title, const std::vector<std::string_view>& stops);

        const Bus* GetBus(std::string_view title) const;
        BusStats GetBusStats(std::string_view title) const;
        const std::unordered_set<const Bus*>& GetBusesOfStop(std::string_view title) const;
        const Stop* GetStop(std::string_view title) const;
    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;

        std::unordered_map<std::string_view, const Stop*> stops_index_;
        std::unordered_map<std::string_view, const Bus*> buses_index_;
        
        std::unordered_map<std::string_view, std::unordered_set<const Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<std::string, std::string>, int, PairHash> stops_to_distance_;
    };
}



