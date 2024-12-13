#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

class TransportRouter {
public:
	TransportRouter() = default;

	TransportRouter(const int bus_wait_time, const double bus_velocity)
		: bus_wait_time_(bus_wait_time)
		, bus_velocity_(bus_velocity) {}

	TransportRouter(const TransportRouter& settings, const TransportCatalogue& catalogue) {
		bus_wait_time_ = settings.bus_wait_time_;
		bus_velocity_ = settings.bus_velocity_;
		BuildGraph(catalogue);
	}

	const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);
	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string & stop_from, const std::string & stop_to) const;
	const graph::DirectedWeightedGraph<double>& GetGraph() const;

private:
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;

	graph::DirectedWeightedGraph<double> graph_;
	std::unordered_map<std::string, graph::VertexId> stop_to_id_;
	std::unique_ptr<graph::Router<double>> router_;
};

