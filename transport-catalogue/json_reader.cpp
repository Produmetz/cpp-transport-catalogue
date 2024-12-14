#include "json_reader.h"
#include "json_builder.h"

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsDict().count("base_requests")) return dummy_;
    return input_.GetRoot().AsDict().at("base_requests");
}

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsDict().count("stat_requests")) return dummy_;
    return input_.GetRoot().AsDict().at("stat_requests");
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (!input_.GetRoot().AsDict().count("render_settings")) return dummy_;
    return input_.GetRoot().AsDict().at("render_settings");
}
const json::Node& JsonReader::GetRoutingSettings() const {
    if (!input_.GetRoot().AsDict().count("routing_settings")) return dummy_;
    return input_.GetRoot().AsDict().at("routing_settings");
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") result.emplace_back(PrintStop(request_map, rh).AsDict());
        if (type == "Bus") result.emplace_back(PrintRoute(request_map, rh).AsDict());
        if (type == "Map") result.emplace_back(PrintMap(request_map, rh).AsDict());
        if (type == "Route") result.push_back(PrintRouting(request_map, rh).AsDict());
    }

    json::Print(json::Document{ result }, std::cout);
}

void JsonReader::FillCatalogue(TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            ToStopInfo info = FillStop(request_stops_map);
            catalogue.AddStop(info.stop_.name_, info.stop_.coordinates_);
        }
    }
    FillStopDistances(catalogue);

    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map);
            catalogue.AddBus(bus_number, stops, circular_route);
        }
    }
}

ToStopInfo JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsDict();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return {{std::string{stop_name}, coordinates}, stop_distances};
    //std::make_tuple(stop_name, coordinates, stop_distances);
}

void JsonReader::FillStopDistances(TransportCatalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops: arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            ToStopInfo info = FillStop(request_stops_map);
            for (auto& [to_name, dist] : info.distance_to_stops_) {
                catalogue.AddDistance(info.stop_.name_, std::string{to_name}, dist);
            }
        }
    }
}

AlmostBus JsonReader::FillRoute(const json::Dict& request_map/*, TransportCatalogue& catalogue*/) const {
    std::string bus_number = request_map.at("name").AsString();
    std::vector<std::string_view> names_stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        names_stops.emplace_back(std::string_view{stop.AsString()});
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();

    return {bus_number, names_stops, circular_route}; 
    //std::make_tuple(bus_number, stops, circular_route);
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Node& settings) const {
    json::Dict request_map = settings.AsDict();
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
    
    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        } else throw std::logic_error("wrong underlayer colortype");
    } else throw std::logic_error("wrong underlayer color");
    
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();
    
    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.emplace_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.emplace_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            } else throw std::logic_error("wrong color_palette type");
        } else throw std::logic_error("wrong color_palette");
    }
    
    return render_settings;
}

RoutingSettings JsonReader::FillRoutingSettings(const json::Node& settings) const {
    //RoutingSettings routing_settings;
    return RoutingSettings{settings.AsDict().at("bus_velocity").AsDouble(), settings.AsDict().at("bus_wait_time").AsInt()};
}


const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    const std::string& route_number = request_map.at("name").AsString();
    const int request_id = request_map.at("id").AsInt();
    
    if (!rh.IsBusNumber(route_number)) {
        return  json::Node {json::Builder{}.StartDict()
                .Key("request_id").Value(request_id)
                .Key("error_message").Value("not found")
            .EndDict()
        .Build()};
    }
    auto bus_info = rh.GetBusStat(route_number);
    return json::Node {json::Builder{}.StartDict()
            .Key("request_id").Value(request_id)
            .Key("curvature").Value(bus_info->curvature)
            .Key("route_length").Value(bus_info->route_length)
            .Key("stop_count").Value(static_cast<int>(bus_info->stops_on_route))
            .Key("unique_stop_count").Value(static_cast<int>(bus_info->unique_stops))
        .EndDict()
    .Build()};
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
    const std::string& stop_name = request_map.at("name").AsString();
    const int request_id = request_map.at("id").AsInt();
    if (!rh.IsStopName(stop_name)) {
        return json::Node{json::Builder{}.StartDict()
                .Key("request_id").Value(request_id)
                .Key("error_message").Value("not found")
            .EndDict()
        .Build()};
    }
    
    json::Array buses;
    for (auto& bus : rh.GetBusesByStop(stop_name)) {
        buses.push_back(bus);
    }
    return json::Node{json::Builder{}.StartDict()
            .Key("request_id").Value(request_id)
            .Key("buses").Value(buses)
        .EndDict()
    .Build()};
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
    const int request_id = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);

    return json::Node{json::Builder{}.StartDict()
            .Key("request_id").Value(request_id)
            .Key("map").Value(strm.str())
        .EndDict()
    .Build()};
}

const json::Node JsonReader::PrintRouting(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string_view stop_from = request_map.at("from").AsString();
    const std::string_view stop_to = request_map.at("to").AsString();
    const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);
    
    if (!routing) {
        result = json::Builder{}
            .StartDict()
                .Key("request_id").Value(id)
                .Key("error_message").Value("not found")
            .EndDict()
        .Build();
    }
    else {
        json::Array items;
        double total_time = routing.value().total_weight;
        items.reserve(routing.value().path.size());
        for (const auto& edge : routing.value().path) {
            //router_.GetEdge(edge_id);
            //const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
            //routing.value().
            if (edge.quality == 0) {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("stop_name").Value(edge.name)
                        .Key("time").Value(edge.weight)
                        .Key("type").Value("Wait")
                    .EndDict()
                .Build()));
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("bus").Value(edge.name)
                        .Key("span_count").Value(static_cast<int>(edge.quality))
                        .Key("time").Value(edge.weight)
                        .Key("type").Value("Bus")
                    .EndDict()
                .Build()));
            }
        }

        result = json::Builder{}
            .StartDict()
                .Key("request_id").Value(id)
                .Key("total_time").Value(total_time)
                .Key("items").Value(items)
            .EndDict()
        .Build();
    }

    return result;
};