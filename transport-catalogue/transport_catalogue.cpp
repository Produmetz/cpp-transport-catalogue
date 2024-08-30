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
    
    double length = 0;
    double length_from_coordinates = 0;
    set<string> unique_stops;
    
    
    Bus bus = FindBus(name);
    //static BusInfo result; //{"", "", ""}; 
    // почему-то  businfo возвращалось не пустым, когда должно было
    // а с предыдущим значением 
    if(bus.name_ == "" ){
        return BusInfo{};
    }
    size_t count_unique_stops = 0;
    
    for(size_t i = 0; i < bus.stops_.size(); ++i ){
        if(unique_stops.count(bus.stops_[i]->name_) == 0){
            ++count_unique_stops;
            unique_stops.insert(bus.stops_[i]->name_);
        }
        if(i != (bus.stops_.size()-1)){
            length_from_coordinates += ComputeDistance(bus.stops_[i]->coordinates_, bus.stops_[i + 1]->coordinates_) ;
            if(auto iter = distance_between_stops_.find({bus.stops_[i]->name_, bus.stops_[i + 1]->name_}); iter != distance_between_stops_.end()){
                length += iter->second;
            }else{
                length += distance_between_stops_.at({bus.stops_[i + 1]->name_, bus.stops_[i]->name_});
            }
            
        }
         
    }
    //result = {to_string(bus.stops_.size()), to_string(count_unique_stops), to_string(length)};
    return {bus.stops_.size(), count_unique_stops, length, 1.0*length/length_from_coordinates};
}
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