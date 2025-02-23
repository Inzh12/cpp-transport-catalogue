#pragma once

#include "domain.h"
#include "svg.h"

namespace transport_catalogue {

class SphereProjector;

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
    std::string Render(const std::vector<const Bus*>& buses, const std::vector<const Stop*> stops) const;
private:
    SphereProjector MakeProjector(const std::vector<const Bus*>& buses) const;

    void RenderBusesLines(svg::Document& doc,
                          const std::vector<const Bus*> buses,
                          const SphereProjector& projector) const;

    void RenderBusesTitles(svg::Document& doc,
                           const std::vector<const Bus*> buses,
                           const SphereProjector& projector) const;

    void RenderStops(svg::Document& doc,
                     const std::vector<const Stop*> stops,
                     const SphereProjector& projector) const;

    void RenderStopsTitles(svg::Document& doc,
                           const std::vector<const Stop*> stops,
                           const SphereProjector& projector) const;

    Settings settings_;
};

}
