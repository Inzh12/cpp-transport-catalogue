#include "map_renderer.h"
#include <algorithm>

namespace transport_catalogue {

inline const double EPSILON = 1e-6;
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding)
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

void MapRenderer::SetSettings(Settings settings) {
    settings_ = settings;
}

std::string MapRenderer::Render(const std::vector<const Bus*>& buses,
                                const std::vector<const Stop*> stops) const {

    svg::Document doc;
    SphereProjector projector = MakeProjector(buses);

    RenderBusesLines(doc, buses, projector);
    RenderBusesTitles(doc, buses, projector);
    RenderStops(doc, stops, projector);
    RenderStopsTitles(doc, stops, projector);

    std::stringstream render_result;
    doc.Render(render_result);
    return render_result.str();
}

SphereProjector MapRenderer::MakeProjector(const std::vector<const Bus *>& buses) const {
    std::vector<geo::Coordinates> coords;
    for (const Bus* bus : buses) {
        for (const Stop* stop : bus->stops) {
            coords.push_back(stop->coords);
        }
    }

    return {coords.begin(), coords.end(), settings_.width, settings_.height, settings_.padding};
}

void MapRenderer::RenderBusesLines(svg::Document& doc,
                                   const std::vector<const Bus *> buses,
                                   const SphereProjector& projector) const {
    int palette_index = 0;
    for (const Bus* bus : buses) {
        if(bus->stops.empty()) {
            continue;
        }

        svg::Polyline line;
        line.SetStrokeColor(settings_.color_palette.at(palette_index))
            .SetFillColor(svg::NoneColor)
            .SetStrokeWidth(settings_.line_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        for (const Stop* stop : bus->stops) {
            line.AddPoint(projector(stop->coords));
        }

        if(palette_index == static_cast<int>(settings_.color_palette.size()) - 1) {
            palette_index = 0;
        } else {
            ++palette_index;
        }

        doc.Add(line);
    }
}

void MapRenderer::RenderBusesTitles(svg::Document &doc,
                                    const std::vector<const Bus *> buses,
                                    const SphereProjector &projector) const {

    int palette_index = 0;
    for (const Bus* bus : buses) {
        if (bus->stops.empty()) {
            continue;
        }

        svg::Text bus_title;

        bus_title.SetData(bus->title)
                 .SetPosition(projector(bus->stops.front()->coords))
                 .SetOffset({settings_.bus_label_offset.x, settings_.bus_label_offset.y})
                 .SetFontSize(settings_.bus_label_font_size)
                 .SetFontFamily("Verdana")
                 .SetFontWeight("bold");

        svg::Text bus_title_base = bus_title;

        bus_title_base.SetFillColor(settings_.underlayer_color)
                      .SetStrokeColor(settings_.underlayer_color)
                      .SetStrokeWidth(settings_.underlayer_width)
                      .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                      .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        bus_title.SetFillColor(settings_.color_palette.at(palette_index));

        if(palette_index == static_cast<int>(settings_.color_palette.size()) - 1) {
            palette_index = 0;
        } else {
            ++palette_index;
        }

        doc.Add(bus_title_base);
        doc.Add(bus_title);

        const Stop* last = bus->stops.at(bus->stops.size()/2);

        if (!bus->is_roundtrip && bus->stops.front()->title != last->title) {
            svg::Text bus_title2 = bus_title;
            svg::Text bus_title_base2 = bus_title_base;

            bus_title2.SetPosition(projector(last->coords));
            bus_title_base2.SetPosition(projector(last->coords));

            doc.Add(bus_title_base2);
            doc.Add(bus_title2);
        }
    }
}

void MapRenderer::RenderStops(svg::Document &doc,
                              const std::vector<const Stop*> stops,
                              const SphereProjector &projector) const {

    for (const Stop* stop : stops) {
        svg::Circle circle;
        circle.SetCenter(projector(stop->coords))
              .SetRadius(settings_.stop_radius)
              .SetFillColor("white");
        doc.Add(circle);
    }

}

void MapRenderer::RenderStopsTitles(svg::Document &doc,
                                    const std::vector<const Stop *> stops,
                                    const SphereProjector &projector) const {

    for (const Stop* stop : stops) {
        svg::Text stop_title;

        stop_title.SetData(stop->title)
                 .SetPosition(projector(stop->coords))
                 .SetOffset({settings_.stop_label_offset.x, settings_.stop_label_offset.y})
                 .SetFontSize(settings_.stop_label_font_size)
                 .SetFontFamily("Verdana");

        svg::Text stop_title_base = stop_title;

        stop_title_base.SetFillColor(settings_.underlayer_color)
                       .SetStrokeColor(settings_.underlayer_color)
                       .SetStrokeWidth(settings_.underlayer_width)
                       .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                       .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        stop_title.SetFillColor("black");

        doc.Add(stop_title_base);
        doc.Add(stop_title);
    }
}

}
