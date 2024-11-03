#pragma once

#include "geo.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <set>

namespace transport_catalogue {
    struct Stop;

    struct Bus {
        std::string title;
        std::vector<Stop const *> stops;
        int stops_amount = 0;
        int uniq_stops_amount = 0;
        double route_length = 0;
    };

    struct BusPoinerComapare {
        bool operator()(const Bus * const lhs, const Bus * const rhs) const {
            return lhs->title < rhs->title;
        }
    };

    struct PairOfPoinersHash {
        size_t operator()(const std::pair<const Bus * const, const Bus * const>& pair) const {
            return std::hash<const void *>{}(pair.first) ^ std::hash<const void *>{}(pair.second);
        }
    };

    struct Stop {
        std::string title;
        geo::Coordinates coords;
        std::set<Bus const *, BusPoinerComapare> buses;
    };

    class TransportCatalogue {
    public:
        void AddStop(std::string_view title, geo::Coordinates coords);
        void AddBus(std::string_view title, const std::vector<std::string_view>& stops);
        const Bus& GetBus(std::string_view title) const;
        const Stop& GetStop(std::string_view title) const;
        bool ContainsBus(std::string_view title) const;
        bool ContainsStop(std::string_view title) const;
    private:
        std::unordered_map<std::string, Stop> stops_;
        std::unordered_map<std::string, Bus> buses_;
        std::unordered_map<std::pair<const Bus* const, const Bus* const>, int, PairOfPoinersHash> distances_;
    };
}



