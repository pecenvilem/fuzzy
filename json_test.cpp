#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include\json.hpp"
#include "include\fuzzy.h"

using json = nlohmann::json;


int main(int argc, char **argv)
{
    // map_type map;
    // map["LinearCurve"] = &createInstance<LinearCurve>;

    // LinearCurve c(0, 10, 1, 2);
    // json j = c;

    // std::cout << j.dump(4) << std::endl;

    // auto c2 = j.get<LinearCurve>();

    std::ifstream infile(".\\test.json");

    json j_from_file = json::parse(infile);
    infile.close();

    std::cout << "Loaded from file:" << std::endl;
    std::cout << j_from_file.dump(4) << std::endl;
    // auto c3 = j_from_file[0].get<FuzzySet>();

    FuzzySet s(j_from_file[0]);
    std::cout << s.membership(+20) << std::endl;

    // std::cout << c3.membership(10) << std::endl;
    
    return 0;
}
