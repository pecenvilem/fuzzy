#pragma once

#include "json.hpp"
#include <map>
#include <fstream>
#include <vector>
#include <string>

#include "curves.h"

using json = nlohmann::json;

class FuzzySet
{
        friend void to_json(json &j, const FuzzySet &set);
        friend void from_json(const json &j, FuzzySet &set);
    private:
        std::string _name = "";
        std::vector<Curve*> _curves;
    public:
        FuzzySet(void) {};
        FuzzySet(const std::string name, const std::vector<Curve*> curves);
        FuzzySet(const std::string name, const json &j_curves);
        FuzzySet(const json &j);
        ~FuzzySet(void);
        double membership(double value);
        void generate_plot_data(
            std::string filename, int samples = 300,
            double center_infinite = 0, double lookahead_infinite = 10
        );
    private:
        void _get_curves_from_json(const json &j);
        bool _is_finite(void);
        double _min_bound(void);
        double _max_bound(void);
        double _span(void);
};
