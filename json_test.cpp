#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include\json.hpp"
#include "include\fuzzy.h"

using json = nlohmann::json;


int main(int argc, char **argv)
{

    std::ifstream infile(".\\test.json");

    json j_from_file = json::parse(infile);
    infile.close();

    FuzzySet s(j_from_file[0]);
    s.generate_plot_data("Cold.csv");
    system("CALL plot\\plot.bat Cold.csv");
    
    return 0;
}
