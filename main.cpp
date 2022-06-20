#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include\json.hpp"
#include "include\fuzzy.h"
#include "include\app.hpp"

using json = nlohmann::json;

int main(int argc, char **argv)
{
    App app;
    app.loop();
    return 0;
}
