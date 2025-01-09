#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>


namespace transport_catalogue::io {
    /**
     * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
     */
    geo::Coordinates ParseCoordinates(std::string_view str, std::string::size_type& pos) {
        static const double nan = std::nan("");

        auto not_space = str.find_first_not_of(' ', pos);
        auto comma = str.find(',');

        if (comma == str.npos) {
            return {nan, nan};
        }

        auto not_space2 = str.find_first_not_of(' ', comma + 1);
        auto end = str.find_first_of(",\n", not_space2);

        double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
        double lng = std::stod(std::string(str.substr(not_space2, end - not_space2)));

        pos = end;

        return {lat, lng};
    }

    std::map<std::string, int> ParseDistances(std::string_view str, std::string::size_type& pos) {
        std::map<std::string, int> stop_to_dist;

        while (pos != std::string::npos) {
            auto not_space = str.find_first_not_of(" ,", pos);
            auto separator = str.find("m to ", pos);
            auto not_space2 = str.find_first_not_of(' ', separator + 5);
            auto separator2 = str.find_first_of(",\n", not_space2);

            int dist = std::stoi(std::string(str.substr(not_space, separator - not_space)));
            std::string stop = std::string(str.substr(not_space2, separator2 - not_space2));

            stop_to_dist.insert({stop, dist});

            pos = separator2;
        }

        return stop_to_dist;
    }

    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    std::vector<std::string_view> Split(std::string_view string, char delim) {
        std::vector<std::string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }

    /**
     * Парсит маршрут.
     * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
     * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
     */
    std::vector<std::string_view> ParseRoute(std::string_view route) {
        if (route.find('>') != route.npos) {
            return Split(route, '>');
        }

        auto stops = Split(route, '-');
        std::vector<std::string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

        return results;
    }

    CommandDescription ParseCommandDescription(std::string_view line) {
        auto colon_pos = line.find(':');
        if (colon_pos == line.npos) {
            return {};
        }

        auto space_pos = line.find(' ');
        if (space_pos >= colon_pos) {
            return {};
        }

        auto not_space = line.find_first_not_of(' ', space_pos);
        if (not_space >= colon_pos) {
            return {};
        }

        return {std::string(line.substr(0, space_pos)),
                std::string(line.substr(not_space, colon_pos - not_space)),
                std::string(line.substr(colon_pos + 1))};
    }

    void InputReader::ParseLine(std::string_view line) {
        auto command_description = ParseCommandDescription(line);
        if (command_description) {
            commands_.push_back(std::move(command_description));
        }
    }

    void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
        std::vector<CommandDescription> commands = commands_;
        std::sort(commands.begin(), commands.end(),
                  [](const CommandDescription& lhs, const CommandDescription& rhs){
            return lhs.command > rhs.command;
        });

        for (const CommandDescription& command : commands) {
            if (command.command == "Stop") {
                std::string::size_type pos = 0;
                auto coords = ParseCoordinates(command.description, pos);
                auto dists = ParseDistances(command.description, pos);
                catalogue.AddStop(command.id, coords);

                for (const auto& [stop, dist] : dists) {
                    catalogue.SetStopsDistance(command.id, stop, dist);
                }
            } else {
                catalogue.AddBus(command.id, ParseRoute(command.description));
            }
        }
    }
}


