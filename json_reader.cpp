#include "json_reader.h"
#include "json.h"
#include <cmath>
#include <set>

namespace transport_catalogue {

JsonReader::JsonReader(TransportCatalogue& catalogue,
                       RequestHandler &request_hander,
                       MapRenderer& renderer,
                       std::istream &input)
    : catalogue_(catalogue),
      request_hander_(request_hander),
      renderer_(renderer)
{
    json::Document json_doc = json::Load(input);
    root_ = json_doc.GetRoot().AsMap();
}

void JsonReader::FillCatalogue() {
    const json::Array& base_requests = root_.at("base_requests").AsArray();

    for (const json::Node& base_request_node : base_requests) {
        const json::Dict& base_request = base_request_node.AsMap();

        if (base_request.at("type") == "Stop") {
            const std::string& stop_name = base_request.at("name").AsString();
            double latitude = base_request.at("latitude").AsDouble();
            double longitude = base_request.at("longitude").AsDouble();

            catalogue_.AddStop(stop_name, {latitude, longitude});

            if (base_request.contains("road_distances")) {
                const json::Dict& road_distances = base_request.at("road_distances").AsMap();

                for (auto& [destination_stop_name, distance_node] : road_distances) {
                    catalogue_.SetStopsDistance(stop_name, destination_stop_name, distance_node.AsDouble());
                }
            }
         }
    }

    for (const json::Node& base_request_node : base_requests) {
        const json::Dict& base_request = base_request_node.AsMap();

        if (base_request.at("type") == "Bus") {
            const std::string& bus_name = base_request.at("name").AsString();
            bool is_roundtrip = base_request.at("is_roundtrip").AsBool();
            const json::Array& stops = base_request.at("stops").AsArray();

            std::vector<std::string_view> stops_vec;
            for (const json::Node& stop_name_node : stops) {
                stops_vec.push_back(stop_name_node.AsString());
            }

            request_hander_.AddBus(bus_name, stops_vec, is_roundtrip);
        }
    }
}

void JsonReader::PrintStats(std::ostream& output) {
    const json::Array& stat_requests = root_.at("stat_requests").AsArray();
    json::Array requests{};

    for (const json::Node& stat_request_node : stat_requests) {
        const json::Dict& stat_request = stat_request_node.AsMap();
        const int id = stat_request.at("id").AsInt();
        json::Dict stat{};
        stat["request_id"] = id;

        if (stat_request.at("type") == "Stop") {
            const std::string& stop_name = stat_request.at("name").AsString();
            if(!catalogue_.GetStop(stop_name)) {
                stat["error_message"] = "not found";
                requests.push_back(std::move(stat));
                continue;
            }

            json::Array buses_array;
            std::set<std::string> buses;
            for (const auto bus_ptr : catalogue_.GetBusesOfStop(stop_name)) {
                buses.insert(bus_ptr->title);
            }

            for (const std::string& bus_title : buses) {
                buses_array.push_back(bus_title);
            }

            stat["buses"] = buses_array;

            requests.push_back(std::move(stat));
        } else if (stat_request.at("type") == "Bus") {
            const std::string& bus_name = stat_request.at("name").AsString();

            BusStats stats;
            if(auto val = catalogue_.GetBusStats(bus_name)){
                stats = val.value();
            } else {
                stat["error_message"] = "not found";
                requests.push_back(std::move(stat));
                continue;
            }

            stat["curvature"] = stats.curvature;
            stat["route_length"] = stats.route_length;
            stat["stop_count"] = stats.stops_amount;
            stat["unique_stop_count"] = stats.uniq_stops_amount;

            requests.push_back(std::move(stat));
        } else if (stat_request.at("type") == "Map") {
            SetRenderSettings();
            stat["map"] = request_hander_.RenderMap();

            requests.push_back(std::move(stat));
        }
    }

    json::Document output_doc{requests};
    Print(output_doc, output);
}

void JsonReader::SetRenderSettings() {
    const json::Dict& render_settings = root_.at("render_settings").AsMap();

    MapRenderer::Settings settings;
    settings.width = render_settings.at("width").AsDouble();
    settings.height = render_settings.at("height").AsDouble();
    settings.padding = render_settings.at("padding").AsDouble();
    settings.line_width = render_settings.at("line_width").AsDouble();
    settings.stop_radius = render_settings.at("stop_radius").AsDouble();
    settings.bus_label_font_size = render_settings.at("bus_label_font_size").AsDouble();

    json::Array bus_label_offset = render_settings.at("bus_label_offset").AsArray();
    settings.bus_label_offset = {bus_label_offset[0].AsDouble(),
                                 bus_label_offset[1].AsDouble()};

    settings.stop_label_font_size = render_settings.at("stop_label_font_size").AsDouble();

    json::Array stop_label_offset = render_settings.at("stop_label_offset").AsArray();
    settings.stop_label_offset = {stop_label_offset[0].AsDouble(),
                                  stop_label_offset[1].AsDouble()};

    settings.underlayer_width = render_settings.at("underlayer_width").AsDouble();
    settings.underlayer_color = ReadColor(render_settings.at("underlayer_color"));

    json::Array color_palette = render_settings.at("color_palette").AsArray();
    for (json::Node color_node : color_palette) {
         settings.color_palette.push_back(ReadColor(color_node));
    }

    renderer_.SetSettings(std::move(settings));
}

svg::Color JsonReader::ReadColor(const json::Node& color_node) {
    if (color_node.IsString()) {
        return color_node.AsString();
    } else if (color_node.IsArray()) {
        json::Array rgb = color_node.AsArray();
        if (rgb.size() == 3) {
            return svg::Rgb{
                static_cast<uint8_t>(rgb[0].AsInt()),
                static_cast<uint8_t>(rgb[1].AsInt()),
                static_cast<uint8_t>(rgb[2].AsInt())
            };
        } else if (rgb.size() == 4) {
            return svg::Rgba{
                static_cast<uint8_t>(rgb[0].AsInt()),
                static_cast<uint8_t>(rgb[1].AsInt()),
                static_cast<uint8_t>(rgb[2].AsInt()),
                rgb[3].AsDouble()
            };
        }
    }

    return svg::NoneColor;
}

}


