#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;
    //freopen ("tsB_case1_input.txt", "r", stdin);
    //freopen ("output.txt", "w", stdout);
    int base_request_count;
    cin >> base_request_count >> ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }
    
    int stat_request_count;
    cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
    //fclose (stdin);
    //fclose (stdout);
    
}