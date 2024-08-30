#include "request_handler.h"

std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    BusInfo bus_info{};
    const Bus bus = catalogue_.FindBus(bus_number);

    if (bus.name_.empty()) std::invalid_argument("bus not found");
    if (bus.is_circle) bus_info.stops_on_route = bus.stops_.size();
    else bus_info.stops_on_route = bus.stops_.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus.stops_.size() - 1; ++i) {
        auto from = bus.stops_[i];
        auto to = bus.stops_[i + 1];
        if (bus.is_circle) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates_,
                to->coordinates_);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates_,
                to->coordinates_) * 2;
        }
    }

    bus_info.unique_stops = catalogue_.UniqueStopsCount(bus_number);
    bus_info.route_length = route_length;
    bus_info.curvature = route_length / geographic_length;

    return bus_info;//catalogue_.GetBusInfo(bus_number);
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.GetBusesForStop(stop_name);
}

bool RequestHandler::IsBusNumber(const std::string_view bus_number) const {
    return !catalogue_.FindBus(bus_number).name_.empty();
}

bool RequestHandler::IsStopName(const std::string_view stop_name) const {
    return !catalogue_.FindStop(stop_name).name_.empty();
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}