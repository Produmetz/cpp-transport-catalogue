#pragma once

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
#include <string>
#include <vector>
#include <map>
#include "geo.h"

 

struct BusInfo {
    explicit operator bool() const {
        return stops_on_route == 0;
    }

    bool operator!() const {
        return !operator bool();
    }

    size_t stops_on_route = 0;      
    size_t unique_stops;           
    double route_length; 
    double curvature; 
};


struct Stop{
    std::string name_;
    geo::Coordinates coordinates_;
};
struct Bus{
    std::string name_;
    std::vector<Stop*> stops_; 
    bool is_circle;   
};
struct AlmostBus{
    std::string name_;
    std::vector<std::string_view> names_stops_; 
    bool is_circle;   
};

struct ToStopInfo {
    Stop stop_;
    /*distance fron stop_ */
    std::map<std::string_view, int> distance_to_stops_;
};