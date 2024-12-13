#include "transport_catalogue.h"

/*
 * Здесь можно разместить код транспортного справочника
 */
//#include "transport_catalogue.h"
#include <unordered_set>
#include <iostream>
#include <stdexcept>

using namespace std;

template<typename K, typename V>
void print_map(unordered_map<K, V> const &m)
{
    for (auto const &pair: m) {
        cout << "{" << pair.first << ": " << pair.second << "}\n";
    }
}
/*
TransportCatalogue::TransportCatalogue() {
    // Конструктор
}

TransportCatalogue::~TransportCatalogue() {
    // Деструктор
}
*/
void TransportCatalogue::AddDistance(const std::string& from_stop, const std::string& to_stop, int distance){
    distance_between_stops_.insert({{from_stop, to_stop}, distance});
};


void TransportCatalogue::AddStop(const string& name, const geo::Coordinates coordinates) {
    
    if(auto iter = reference_to_stops_.find(name); iter != reference_to_stops_.end()){
        (*(iter->second)).coordinates_ = coordinates;
    }else{
        stops_.push_back({name, coordinates});
        reference_to_stops_.insert({name, &stops_.back()});
    }
    stop_to_buses_.insert({name, {}});

}

void TransportCatalogue::AddBus(const string& name, const vector<string_view>& stops, bool is_circle ) {
   
    Bus bus;
    bus.name_ = name;
    bus.is_circle = is_circle;
    for(const auto &name_stop : stops){
        if (reference_to_stops_.count(string{name_stop}) == 0) {
            AddStop(string{name_stop}, geo::Coordinates{});
        }
        bus.stops_.push_back(reference_to_stops_.at(string{name_stop}));
        stop_to_buses_[string{name_stop}].insert(name);
        
    }
    buses_.push_back(bus);
    this->reference_to_buses_.insert({name, &buses_.back()});
}

const Stop &TransportCatalogue::FindStop(string_view name) const {
    
    if(auto iter = reference_to_stops_.find(string{name}); iter != reference_to_stops_.end()){
        return *(iter->second);
    }
    static Stop empty_result = {};
    return empty_result;
    
}

const Bus &TransportCatalogue::FindBus(string_view name) const {
    
    if(auto iter = reference_to_buses_.find(string{name}); iter != reference_to_buses_.end()){
        return *(iter->second);
    }
    static Bus empty_result = {};
    return empty_result;
   
}

int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    /*if (stop_distances_.count({ from, to })) return stop_distances_.at({ from, to });
    else if (stop_distances_.count({ to, from })) return stop_distances_.at({ to, from });
    else return 0;*/

    if(auto iter = distance_between_stops_.find({from->name_, to->name_}); iter != distance_between_stops_.end()){
        return iter->second;
    }else if(auto iter = distance_between_stops_.find({to->name_, from->name_}); iter != distance_between_stops_.end()){
        return iter->second;
    }
    return 0;
}
int TransportCatalogue::GetDistanceBetweenBusStops(const Bus* bus , const Stop* from, const Stop* to, bool IsForward)  const {
    int result = 0;
    bool AreRequiredStops = false;
    auto iter_begin = IsForward ? bus->stops_.begin(): bus->stops_.end();
    auto iter_end = IsForward ? bus->stops_.end(): bus->stops_.begin();
    int increment = IsForward ? 1 : - 1;
    while (iter_begin != iter_end)
    {
        if(AreRequiredStops){
            if(*iter_begin == to){
                return result;
            }
            result += GetDistance(*iter_begin, *(iter_begin + increment));
        }else if(*iter_begin == from){
            AreRequiredStops = true;
        }
        iter_begin += increment;
    }
    

    /*
    for(size_t i = ; i < bus->stops_.size(); ++i){
        if(AreRequiredStops){
            if(bus->stops_[i] == to){
                return result;
            }
            result += GetDistance(bus->stops_[i], bus->stops_[i + 1]);
        }else if(bus->stops_[i] == from){
            AreRequiredStops = true;
        }
    }*/
    return result;
};

size_t TransportCatalogue::UniqueStopsCount(std::string_view bus_number) const {
    std::unordered_set<std::string_view> unique_stops;
    if(auto iter = reference_to_buses_.find(string{bus_number}); iter != reference_to_buses_.end()){
        for (const auto& stop : (iter->second)->stops_) {
            unique_stops.insert(stop->name_);
        }
    }
    return unique_stops.size();
}

BusInfo TransportCatalogue::GetBusInfo(string_view name) const {
    
    BusInfo bus_info{};
    const Bus bus = this->FindBus(name);

    if (bus.name_.empty()) std::invalid_argument("bus not found");
    if (bus.is_circle) bus_info.stops_on_route = bus.stops_.size();
    else bus_info.stops_on_route = bus.stops_.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus.stops_.size() - 1; ++i) {
        auto from = bus.stops_[i];
        auto to = bus.stops_[i + 1];
        if (bus.is_circle) {
            route_length += this->GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates_,
                to->coordinates_);
        }
        else {
            route_length += this->GetDistance(from, to) + this->GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates_,
                to->coordinates_) * 2;
        }
    }

    bus_info.unique_stops = this->UniqueStopsCount(name);
    bus_info.route_length = route_length;
    bus_info.curvature = route_length / geographic_length;

    return bus_info;
}

size_t TransportCatalogue::GetCountStops() const {
    return stops_.size();
};

const set<string> & TransportCatalogue::GetBusesForStop(string_view stop_name) const {
    
    if(auto iter = stop_to_buses_.find(string{stop_name}); iter != stop_to_buses_.end()){
        return (iter->second);
    }
    static set<string> result = {};
    return  result;
};
const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedAllBuses() const {
    std::map<std::string_view, const Bus*> result;
    for (const auto& bus : reference_to_buses_) {
        result.emplace(bus);
    }
    return result;
}
const std::vector<const Stop*> TransportCatalogue::GetAllStops() const {
    std::vector<const Stop*> result;
    for (size_t i = 0; i < stops_.size(); ++i) {
        result.emplace_back(&stops_[i]);
    }
    return result;
}