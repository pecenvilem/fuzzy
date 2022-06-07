#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>

#include "include\fuzzy.h"

int	main(int argc, char **argv)
{
    using json = nlohmann::json;

    typedef std::map<std::string, std::string> mp;

    //std::ofstream file("test.json");

    LinearCurve c(0, 10, 1, 0);


    //file.close();

    system("pause");
    return 0;
}
