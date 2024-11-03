#include "stat_reader.h"
#include "input_reader.h"
#include <stdexcept>

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
        if (!tansport_catalogue_.ContainsBus(bus_id)) {
            output_ << "Bus "sv << bus_id << ": not found\n"sv;
            return;
        }

        const Bus& bus = tansport_catalogue_.GetBus(bus_id);

        output_ << "Bus "sv
                << bus.title
                << ": "sv
                << std::to_string(bus.stops_amount)
                << " stops on route, "sv
                << std::to_string(bus.uniq_stops_amount)
                << " unique stops, "sv
                << std::to_string(bus.route_length)
                << " route length\n"sv;
    }

    void StatReader::PrintStopStat(std::string_view stop_id)
    {
        if (!tansport_catalogue_.ContainsStop(stop_id)) {
            output_ << "Stop "sv << stop_id << ": not found\n"sv;
            return;
        }

        const Stop& stop = tansport_catalogue_.GetStop(stop_id);

        output_ << "Stop "sv << stop.title;

        if(stop.buses.empty()) {
            output_ << ": no buses\n"sv;
        } else {
            output_ << ": buses "sv;

            for (const Bus* bus : stop.buses) {
                output_ << bus->title << " "sv;
            }

            output_ << "\n"sv;
        }
    }
}
