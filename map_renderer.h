#pragma once

#include "domain.h"
#include "svg.h"

#include <deque>

namespace transport_catalogue {

class MapRenderer {
public:
    struct Offset
    {
        double x = 0;
        double y = 0;
    };

    struct Settings {
        double width = 0;
        double height = 0;
        double padding = 0;
        double line_width = 0;
        double stop_radius = 0;
        int bus_label_font_size = 0;
        Offset bus_label_offset;
        int stop_label_font_size = 0;
        Offset stop_label_offset;
        svg::Color underlayer_color;
        double underlayer_width = 0;
        std::vector<svg::Color> color_palette;
    };

    void SetSettings(Settings settings);
    std::string Render(const std::deque<Bus>& buses, const std::vector<const Stop*> stops) const;
private:
    Settings settings_;
};

}
