// место для вашего кода
#include "transport_catalogue.h"
#include <iostream>
#include <stdexcept>
using namespace std;
template<typename K, typename V>
void print_map(std::unordered_map<K, V> const &m)
{
    for (auto const &pair: m) {
        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
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
void TransportCatalogue::AddStop(const std::string& name) {
    // Реализация метода
    stops_.push_back({name, {}});
    reference_to_stops_.insert({name, &stops_.back()});
}
void TransportCatalogue::AddStop(const std::string& name, const Coordinates coordinates) {
    // Реализация метода
    if (reference_to_stops_.count(name) == 1) {
        auto stop = reference_to_stops_.at(name);
        stop->coordinates_ = coordinates;
    }else{
        stops_.push_back({name, coordinates});
        reference_to_stops_.insert({name, &stops_.back()});
    }
    stop_to_buses_.insert({name, {}});
    //print_map(reference_to_stops_);
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string_view>& stops) {
    // Реализация метода
    Bus bus;
    bus.name_ = name;
    for(const auto &name_stop : stops){
        if (reference_to_stops_.count(std::string{name_stop}) == 0) {
            AddStop(std::string{name_stop});
        }
        bus.stops_.push_back(reference_to_stops_.at(std::string{name_stop}));
        stop_to_buses_[std::string{name_stop}].insert(name);
        /*for (auto it = stop_to_buses_[std::string{name_stop}].begin(); it !=stop_to_buses_[std::string{name_stop}].end(); ++it){
            cout <<" set "<< *it;
        }
        cout<<endl;*/
        //bus.stops_.push_back(reference_to_stops_.at(std::string{name_stop}));
    }
    buses_.push_back(bus);
    this->reference_to_buses_.insert({name, &buses_.back()});
    //print_map(reference_to_buses_);
}

Stop TransportCatalogue::FindStop(const std::string& name) const {
    // Реализация метода
    if(auto iter = reference_to_stops_.find(name); iter != reference_to_stops_.end()){
        return *(iter->second);
    }
    return Stop{};
    
}

Bus TransportCatalogue::FindBus(const std::string& name) const {
    // Реализация метода
    //print_map(reference_to_buses_);
    
    if(auto iter = reference_to_buses_.find(name); iter != reference_to_buses_.end()){
        return *(iter->second);
    }
    return Bus{};
   
}

std::vector<std::string> TransportCatalogue::GetBusInfo(const std::string& name) const {
    // Реализация метода
    double length = 0.00;
    std::set<string> unique_stops;
    //std::vector<std::string> result = {name};
    
    Bus bus = FindBus(name);
    //cout<<bus.name_<<endl;
    if(bus.name_ == "" ){
        return {};
    }
    int count_unique_stops = 0;//bus.stops_.size();
    //result.push_back(std::to_string(count_unique_stops));
    for(size_t i = 0; i < bus.stops_.size(); ++i /*auto &&stop : bus.stops_*/){
        if(unique_stops.count(bus.stops_[i]->name_) == 0){
            ++count_unique_stops;
            unique_stops.insert(bus.stops_[i]->name_);
        }
        if(i != (bus.stops_.size()-1)){
            length += ComputeDistance(bus.stops_[i]->coordinates_, bus.stops_[i + 1]->coordinates_) ;
        }
         
    }
    return {name, std::to_string(bus.stops_.size()), std::to_string(count_unique_stops), std::to_string(length)};//std::nullopt;
}
std::set<std::string> TransportCatalogue::GetBusesForStop(const std::string& stop_name) const {
    
    if(auto iter =  stop_to_buses_.find(stop_name); iter != stop_to_buses_.end()){
        return (iter->second);
    }
    return std::set<std::string>{};
};
