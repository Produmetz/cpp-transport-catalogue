#include "transport_router.h"



const graph::DirectedWeightedGraph<double>& TransportRouter::BuildGraph(const TransportCatalogue& catalogue) {
	
	graph::DirectedWeightedGraph<double> stops_graph(catalogue.GetCountStops() * 2);
    const auto & all_buses = catalogue.GetSortedAllBuses();
    //double coeff_from_kmh_to_mmin = 100 / 6;
    double bus_velocity_in_mmin = bus_velocity_ * 100 / 6;
    
    for(const auto &[bus_name, ptr_bus] : all_buses){
        size_t count_stops = ptr_bus->stops_.size();
        bool is_circle = ptr_bus->is_circle;
        for(size_t i = 0; i < count_stops; ++i){
            int length_way = 0,
                length_way_reverse = 0; 
            for( size_t j = i; j < count_stops; ++j){
                if(stop_to_id_.find(ptr_bus->stops_[j]->name_) == stop_to_id_.end()){
                    stop_to_id_.insert({ptr_bus->stops_[j]->name_, stop_to_id_.size() * 2}); // if VertexId == size_t
                    stops_graph.AddEdge({
                        ptr_bus->stops_[j]->name_,
                        0,
                        stop_to_id_[ptr_bus->stops_[j]->name_],
                        stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                        bus_wait_time_ + 0.0
                    });

                }
                if(i != j){
                    length_way += catalogue.GetDistance(ptr_bus->stops_[j - 1], ptr_bus->stops_[j]);
                    length_way_reverse += catalogue.GetDistance(ptr_bus->stops_[j], ptr_bus->stops_[j - 1]);
                    stops_graph.AddEdge({ 
                        std::string{bus_name},
                        j - i,
                        stop_to_id_[ptr_bus->stops_[i]->name_] + 1,
                        stop_to_id_[ptr_bus->stops_[j]->name_],
                        length_way / bus_velocity_in_mmin
                    });
                    if (!is_circle) {
                        stops_graph.AddEdge({ 
                            std::string{bus_name},
                            j - i,
                            stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                            stop_to_id_[ptr_bus->stops_[i]->name_],
                            length_way_reverse / bus_velocity_in_mmin
                        });
                    }
                }
                
            }
        }
        
    }

    
    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}

const std::optional<TransportRouter::PathOfEdges<double>> TransportRouter::FindRoute(const std::string & stop_from, const std::string & stop_to) const {
    auto iter_from = stop_to_id_.find(stop_from); auto iter_to = stop_to_id_.find(stop_to);
    if((iter_from != stop_to_id_.end()) && (iter_to != stop_to_id_.end())){
        auto route_info = router_->BuildRoute((*iter_from).second, (*iter_to).second);
        if(route_info.has_value()){
            //PathOfEdges<double> route{route_info.value().weight, vector};
            std::vector<graph::Edge<double>> edges(route_info.value().edges.size());
            size_t i = 0;
            for(const auto & edge_id : route_info.value().edges){
                edges[i] = graph_.GetEdge(edge_id);
                ++i;
            }
            return {{route_info.value().weight, edges}};
        }
    }
	
    return std::nullopt;
}
/*const graph::Edge<double>& TransportRouter::GetEdge(graph::EdgeId edge_id) const {
    return graph_.GetEdge(edge_id);
};*/

/*const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
	return graph_;
}*/

