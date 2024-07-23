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

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        // Hash the first element
        size_t hash1 = std::hash<T1>{}(p.first);
        // Hash the second element
        size_t hash2 = std::hash<T2>{}(p.second);
        // Combine the two hash values
        return hash1
               ^ (hash2 + 0x9e3779b9 + (hash1 << 6)
                  + (hash1 >> 2));
    }
};


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
    
    
    const Stop &FindStop(std::string_view name) const;
    const Bus &FindBus(std::string_view name) const;
    
    
    BusInfo GetBusInfo(std::string_view name) const;
    const std::set<std::string> &GetBusesForStop(std::string_view stop_name) const;


private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string,  Stop*> reference_to_stops_;
    std::unordered_map<std::string,  Bus*> reference_to_buses_;
    std::unordered_map<std::string, std::set<std::string>> stop_to_buses_;
    std::unordered_map<std::pair<std::string, std::string>,  double, hash_pair> distance_between_stops_;
};