#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace transport_catalogue::io {
    class StatReader {
    public:
        StatReader(const TransportCatalogue& tansport_catalogue, std::ostream& output);

        void ParseAndPrintStat(std::string_view request);
    private:
        void PrintBusStat(std::string_view bus_id);

        void PrintStopStat(std::string_view stop_id);

        const TransportCatalogue& tansport_catalogue_;
        std::ostream& output_;
    };
}
