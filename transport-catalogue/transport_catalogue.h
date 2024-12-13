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
#include <map>

#include "geo.h"
#include "domain.h"


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



class TransportCatalogue {
public:
    //TransportCatalogue() = default();
    //~TransportCatalogue();
    //void AddStop(const std::string& name);
    void AddStop(const std::string& name, const geo::Coordinates coordinates);
    void AddBus(const std::string& name, const std::vector<std::string_view>& stops, bool is_circle);
    void AddDistance(const std::string& from_stop, const std::string& to_stop, int distance);
    
    const Stop &FindStop(std::string_view name) const;
    const Bus &FindBus(std::string_view name) const;
    
    size_t UniqueStopsCount(std::string_view bus_number) const;
    int GetDistance(const Stop* from, const Stop* to) const;
    int GetDistanceBetweenBusStops(const Bus* bus, const Stop* from , const Stop* to, bool IsForward)  const;
    BusInfo GetBusInfo(std::string_view name) const;
    size_t GetCountStops() const ;
    const std::set<std::string> &GetBusesForStop(std::string_view stop_name) const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
    const std::vector<const Stop*> GetAllStops() const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string,  Stop*> reference_to_stops_;
    std::unordered_map<std::string,  Bus*> reference_to_buses_;
    std::unordered_map<std::string, std::set<std::string>> stop_to_buses_;
    std::unordered_map<std::pair<std::string, std::string>,  int, hash_pair> distance_between_stops_;
};