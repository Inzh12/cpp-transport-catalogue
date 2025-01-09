#include "stat_reader.h"

#include <iostream>
#include <set>

using namespace std::literals::string_view_literals;

namespace transport_catalogue::io {
    StatReader::StatReader(const TransportCatalogue& tansport_catalogue, std::ostream& output) 
        : tansport_catalogue_(tansport_catalogue), output_(output) {}

    void StatReader::ParseAndPrintStat(std::string_view request) {
        auto space_pos = request.find_first_of(' ');
        const std::string_view command = request.substr(0, space_pos);
        const std::string_view id = request.substr(space_pos + 1);

        if (command == "Bus"sv) {
            PrintBusStat(id);
        } else if (command == "Stop"sv) {
            PrintStopStat(id);
        }
    }

    void StatReader::PrintBusStat(std::string_view bus_id)
    {
        BusStats stats = tansport_catalogue_.GetBusStats(bus_id);
        if(stats.stops_amount == 0) {
            output_ << "Bus "sv << bus_id << ": not found"sv << std::endl;
            return;
        } 

        output_ << "Bus "sv
                << bus_id
                << ": "sv
                << std::to_string(stats.stops_amount)
                << " stops on route, "sv
                << std::to_string(stats.uniq_stops_amount)
                << " unique stops, "sv
                << std::to_string(stats.route_length)
                << " route length, "sv
                << std::to_string(stats.curvature)
                << " curvature"sv
                << std::endl;
    }

    void StatReader::PrintStopStat(std::string_view stop_id)
    {
        const Stop* stop = tansport_catalogue_.GetStop(stop_id);

        output_ << "Stop "sv << stop_id << ": "sv;

        if(!stop) {
            output_ << "not found"sv << std::endl;
            return;
        }

        auto buses = tansport_catalogue_.GetBusesOfStop(stop->title);

        std::set<std::string> buses_titles;
        for (auto bus : buses) {
            buses_titles.insert(bus->title);
        }

        if(buses.empty()) {
            output_ << "no buses"sv << std::endl;
            return;
        }

        output_ << "buses "sv;

        for (auto bus_title : buses_titles) {
            output_ << bus_title << " "sv;
        }

        output_ << std::endl;
    }
}
