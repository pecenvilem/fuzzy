#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include\json.hpp"
#include "include\fuzzy.h"

using json = nlohmann::json;


int main(int argc, char **argv)
{
    map_type map;
    map["LinearCurve"] = &createInstance<LinearCurve>;

    LinearCurve c(0, 10, 1, 2);
    json j = c;

    std::cout << j.dump(4) << std::endl;

    auto c2 = j.get<LinearCurve>();

    std::ifstream infile(".\\test.json");

    json j_from_file = json::parse(infile);
    infile.close();

    auto c3 = j_from_file[0]["Cold"][1]["LinearCurve"].get<LinearCurve>();

    std::cout << c3.membership(10) << std::endl;

    // system("pause");
    return 0;
}
