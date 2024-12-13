#include "transport_router.h"



const graph::DirectedWeightedGraph<double>& TransportRouter::BuildGraph(const TransportCatalogue& catalogue) {
	//const std::vector<const Stop*> all_pointers_to_stops = catalogue.GetAllStops();
	graph::DirectedWeightedGraph<double> stops_graph(catalogue.GetCountStops() * 2);
    const auto & all_buses = catalogue.GetSortedAllBuses();
    double coef = bus_velocity_ * 100 / 6;
    
    for(const auto &[bus_name, ptr_bus] : all_buses){
        size_t count_stops = ptr_bus->stops_.size();
        bool IsCircle = ptr_bus->is_circle;
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
                        length_way / coef
                    });
                    if (!IsCircle) {
                        stops_graph.AddEdge({ 
                            std::string{bus_name},
                            j - i,
                            stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                            stop_to_id_[ptr_bus->stops_[i]->name_],
                            length_way_reverse / coef
                        });
                    }
                }
                /*else if(i == 0){
                    length_way += catalogue.GetDistance(ptr_bus->stops_[j - 1], ptr_bus->stops_[j]);
                    length_way_reverse += catalogue.GetDistance(ptr_bus->stops_[j], ptr_bus->stops_[j - 1]);
                }
                if(i == j && i != 0){
                    length_way -= catalogue.GetDistance(ptr_bus->stops_[i - 1], ptr_bus->stops_[i - 1]);
                    length_way_reverse -= catalogue.GetDistance(ptr_bus->stops_[j - 1], ptr_bus->stops_[i]);
                }*/
                /*stops_graph.AddEdge({ 
                        std::string{bus_name},
                        j - i,
                        stop_to_id_[ptr_bus->stops_[i]->name_] + 1,
                        stop_to_id_[ptr_bus->stops_[j]->name_],
                        length_way / coef
                    });

                if (!IsCircle) {
                    stops_graph.AddEdge({ 
                        std::string{bus_name},
                        j - i,
                        stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                        stop_to_id_[ptr_bus->stops_[i]->name_],
                        length_way_reverse / coef
                    });
                }*/
            }
        }
        /*for(size_t i = 0; i < count_stops; ++i){
            for(size_t j = i; j < count_stops; ++j){
                if(stop_to_id_.find(ptr_bus->stops_[j]->name_) == stop_to_id_.end()){
                    stop_to_id_.insert({ptr_bus->stops_[j]->name_, stop_to_id_.size() * 2});
                    stops_graph.AddEdge({
                        ptr_bus->stops_[j]->name_,
                        0,
                        stop_to_id_[ptr_bus->stops_[j]->name_],
                        stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                        bus_wait_time_ + 0.0
                    });

                }
                if(){

                }
                if(i == j && i != 0){
                    dist_way -= catalogue.GetDistance(ptr_bus->stops_[i], ptr_bus->stops_[j - 1]);
                    dist_way_reverse -= IsCircle ? 0 : catalogue.GetDistance(ptr_bus->stops_[j - 1], ptr_bus->stops_[i]);
                }else{
                    dist_way += (j != 0)? catalogue.GetDistance(ptr_bus->stops_[j - 1], ptr_bus->stops_[j]): 0 ;
                    dist_way_reverse += (IsCircle || j == 0 )? 0 : catalogue.GetDistance(ptr_bus->stops_[j], ptr_bus->stops_[j - 1]);
                }
                if(i != j){
                    stops_graph.AddEdge({
                        ptr_bus->name_,
                        j - i,
                        stop_to_id_[ptr_bus->stops_[i]->name_] + 1,
                        stop_to_id_[ptr_bus->stops_[j]->name_],
                        dist_way / coef
                    });
                    if(!IsCircle){
                        stops_graph.AddEdge({
                            ptr_bus->name_,
                            j - i,
                            stop_to_id_[ptr_bus->stops_[j]->name_] + 1,
                            stop_to_id_[ptr_bus->stops_[i]->name_],
                            dist_way_reverse / coef
                        });
                    }
                   
                }
                
            }
            if(IsCircle && (i + 1 != count_stops)){
                stops_graph.AddEdge({
                    ptr_bus->name_,
                    i + 1,
                    stop_to_id_[ptr_bus->stops_.back()->name_] + 1,
                    stop_to_id_[ptr_bus->stops_[i]->name_],
                    dist_way_reverse / coef
                });
                dist_way_reverse += catalogue.GetDistance(ptr_bus->stops_[i], ptr_bus->stops_[i + 1]);
            }
        }*/
    }

    
    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}

const std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string & stop_from, const std::string & stop_to) const {
    auto iter_from = stop_to_id_.find(stop_from); auto iter_to = stop_to_id_.find(stop_to);
    if((iter_from != stop_to_id_.end()) && (iter_to != stop_to_id_.end())){
        return router_->BuildRoute((*iter_from).second, (*iter_to).second);
    }
	
    return std::nullopt;
}

const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
	return graph_;
}

