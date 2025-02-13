#pragma once

#include "geo.h"

#include <string>
#include <vector>

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

namespace transport_catalogue {

struct Stop {
    std::string title;
    geo::Coordinates coords;
};

struct Bus {
    std::string title;
    std::vector<const Stop*> stops;
    bool is_roundtrip = false;
};

struct BusStats {
    int stops_amount = 0;
    int uniq_stops_amount = 0;
    int route_length = 0;
    double curvature = 0;
};

struct PairHash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

}
