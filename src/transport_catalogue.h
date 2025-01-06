#pragma once

#include "geo.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <deque>

namespace transport_catalogue {
    struct Stop;

    struct Bus {
        std::string title;
        std::vector<const Stop*> stops;
        int stops_amount = 0;
        int uniq_stops_amount = 0;
        int route_length = 0;
        double curvature = 0;
    };

    struct Stop {
        std::string title;
        geo::Coordinates coords;
    };

    struct BusPoinerComapare {
        bool operator()(const Bus* const lhs, const Bus* const rhs) const {
            return lhs->title < rhs->title;
        }
    };

    class TransportCatalogue {
    public:
        void AddStop(std::string_view title, geo::Coordinates coords,
                     std::vector<std::pair<int, std::string>>& dists_with_stop);

        void AddBus(std::string_view title, const std::vector<std::string_view>& stops);

        const Bus* GetBus(std::string_view title) const;
        const std::set<const Bus*, BusPoinerComapare>& GetBusesOfStop(std::string_view title) const;
        const Stop* GetStop(std::string_view title) const;

    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;

        std::unordered_map<std::string_view, const Stop*> stops_index_;
        std::unordered_map<std::string_view, const Bus*> buses_index_;
        
        std::unordered_map<std::string_view, std::set<const Bus*, BusPoinerComapare>> stop_to_buses_;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> stop_to_distances_;
    };
}



