#include "stat_reader.h"

std::string_view rim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {

    std::string_view request_ = rim(request);
    auto gap =  request_.find(' ');
    std::string_view command = request_.substr(0, gap);
    std::string parameter = std::string{request_.substr(gap + 1)};
    
    if(command == "Bus"){
        std::vector<std::string> info = transport_catalogue.GetBusInfo(parameter);
        if(info.size() == 0){
            output<<request<<": not found"<<std::endl;
        }else{
            output<<request<<": "<<info[0]<<" stops on route, "<<info[1]<<" unique stops, "<<info[2]<<" route length"<<std::endl;
        }
    }else if(command == "Stop"){
        Stop stop_ = transport_catalogue.FindStop(parameter);
        
        if(stop_.name_ != ""){
            std::set<std::string> buses = transport_catalogue.GetBusesForStop(parameter);
            if(!buses.empty()){
                output<<request<<": buses ";
                for(const auto & bus : buses){
                    output<<bus<<" ";
                }
                output<<std::endl;
            }else{
                output<<request<<": no buses"<<std::endl;
            }
        }else{
            output<<request<<": not found"<<std::endl;
        }
    }
    
    
}