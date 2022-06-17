#include "fuzzy.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <math.h>
#include <limits>

FuzzySet::FuzzySet(const std::string name, const std::vector<Curve*> curves)
{
    _name = name; _curves = curves;
}

FuzzySet::FuzzySet(const json &j)
{
    _name = j.begin().key();
    _get_curves_from_json(j.begin().value());
}

FuzzySet::FuzzySet(const std::string name, const json &j_curves)
{
    _name = name;
    _get_curves_from_json(j_curves);
}

FuzzySet::~FuzzySet(void)
{
    for (Curve *c : _curves) delete c;
}

double FuzzySet::membership(double value)
{
    for (Curve *c: _curves) if(c->contains(value)) return c->membership(value);
    return 0;
}

void FuzzySet::generate_plot_data(
    std::string filename, int samples,
    double center_infinite, double lookahead_infinite
)
{
    std::ofstream output_file(filename, std::ios::trunc);
    if (!output_file.good())
    {
        std::string message = "File " + filename + " can't be oppened!";
        throw std::invalid_argument(message);
    }

    double from, to;
    
    #define RENDER(x) ((output_file << x << ";" << c->membership(x) << std::endl))

    for (Curve *c: _curves)
    {
        if (isinf(c->get_lower_bound()))
        {
            if (isinf(c->get_upper_bound()))
            {
                from = center_infinite - lookahead_infinite;
                to = center_infinite + lookahead_infinite;
            } else {
                from = c->get_upper_bound() - lookahead_infinite;
                to = c->get_upper_bound();
            }
        } else {
            if (isinf(c->get_upper_bound()))
            {
                from = c->get_lower_bound();
                to = c->get_lower_bound() + lookahead_infinite;
            } else {
                from = c->get_lower_bound();
                to = c->get_upper_bound();
            }
        }
        RENDER(from);
        for (double i = from; i < to; i += (to - from) / samples) RENDER(i);
        RENDER(to);
    }

    output_file.close();
}

template <typename T> Curve* create_curve(json &j) {return new T(j);}
typedef Curve* (*CurveResolver)(json &j);

void FuzzySet::_get_curves_from_json(const json &j)
{
    std::map<const std::string, CurveResolver> resolver_map = {
        {"ConstantCurve", create_curve<ConstantCurve>},
        {"LinearCurve", create_curve<LinearCurve>},
        {"QuadraticCurve", create_curve<QuadraticCurve>},
        {"ExponentialCurve", create_curve<ExponentialCurve>},
        {"LogarithmicCurve", create_curve<LogarithmicCurve>},
    };
    for (auto element: j)
    {
        std::string s = element.begin().key();
        CurveResolver r = resolver_map.at(element.begin().key());
        _curves.push_back(r(element));
        // auto c = resolver_map["LinearCurve"](element);
        // _curves.push_back(resolver_map[element.begin().key()](element));
        // if (element.begin().key() == "ConstantCurve")
        // {
        //     _curves.push_back(new ConstantCurve(element));
        // }
        // else if (element.begin().key() == "LinearCurve")
        // {
        //     _curves.push_back(new LinearCurve(element));
        // }
    }
}

bool FuzzySet::_is_finite(void)
{
    for (Curve *c: _curves) if(!(c->is_finite())) return false;
    return true;
}

double FuzzySet::_min_bound(void)
{
    if (_curves.size() == 0)
        throw std::out_of_range("Trying to find limit of an empty set!");

    double min = std::numeric_limits<double>::infinity();
    for (Curve *c: _curves)
    {
        if (c->get_lower_bound() < min) min = c->get_lower_bound();
    }
    return min;
}

double FuzzySet::_max_bound(void)
{
    if (_curves.size() == 0)
        throw std::out_of_range("Trying to find limit of an empty set!");
    double max = std::numeric_limits<double>::infinity();
    for (Curve *c: _curves)
    {
        if (c->get_lower_bound() > max) max = c->get_lower_bound();
    }
    return max;
}

double FuzzySet::_span(void)
{
    if (!(_is_finite())) return std::numeric_limits<double>::infinity();
    return _max_bound() - _min_bound();
}
