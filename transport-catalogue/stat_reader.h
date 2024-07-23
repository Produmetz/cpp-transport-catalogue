#pragma once
#include <iostream>
#include <iosfwd>
#include <string_view>


#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);
void ProcessStatistics(const TransportCatalogue& transport_catalogue, std::istream& input, std::ostream& output);