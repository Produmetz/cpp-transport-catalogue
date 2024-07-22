#pragma once
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <utility>
#include <optional>
#include <unordered_map>

#include "geo.h"

struct BusInfo {
    explicit operator bool() const {
        return stops_on_route.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string stops_on_route;      
    std::string unique_stops;           
    std::string route_length;  
};


struct Stop{
    std::string name_;
    Coordinates coordinates_;
};
struct Bus{
    std::string name_;
    std::vector<Stop*> stops_;    
};


class TransportCatalogue {
public:
    //TransportCatalogue() = default();
    //~TransportCatalogue();
    //void AddStop(const std::string& name);
    void AddStop(const std::string& name,const Coordinates coordinates);
    void AddBus(const std::string& name, const std::vector<std::string_view>& stops);
    
    
    Stop FindStop(const std::string& name) const;
    Bus FindBus(const std::string& name) const;
    
    
    BusInfo GetBusInfo(const std::string& name) const;
    const std::set<std::string> & GetBusesForStop(const std::string& stop_name) const;


private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string,  Stop*> reference_to_stops_;
    std::unordered_map<std::string,  Bus*> reference_to_buses_;
    std::unordered_map<std::string, std::set<std::string>> stop_to_buses_;
   
};