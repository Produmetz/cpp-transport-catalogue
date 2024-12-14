#include "request_handler.h"

std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    return catalogue_.GetBusInfo(bus_number);
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

const std::optional<TransportRouter::PathOfEdges<double>> RequestHandler::GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    return router_.FindRoute(std::string(stop_from), std::string(stop_to));
};
