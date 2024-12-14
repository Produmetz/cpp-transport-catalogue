#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

class TransportRouter {
public:
	TransportRouter() = default;
	TransportRouter(const RoutingSettings& settings, const TransportCatalogue& catalogue)
	: bus_wait_time_(settings.bus_wait_time), bus_velocity_(settings.bus_velocity){
		BuildGraph(catalogue);
	}
	template <typename Weight>
	struct PathOfEdges{
		Weight total_weight;
		std::vector<graph::Edge<Weight>> path;
	};

	const std::optional<TransportRouter::PathOfEdges<double>> FindRoute(const std::string & stop_from, const std::string & stop_to) const;
	
private:
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;
	
	graph::DirectedWeightedGraph<double> graph_;
	std::unordered_map<std::string, graph::VertexId> stop_to_id_;
	std::unique_ptr<graph::Router<double>> router_;

	const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);
	
};

