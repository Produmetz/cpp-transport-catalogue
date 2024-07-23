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
    
    {
        InputReader reader(cin);
        reader.ApplyCommands(catalogue);
    }
    ProcessStatistics(catalogue, cin, cout);
    
    //fclose (stdin);
    //fclose (stdout);
    
}