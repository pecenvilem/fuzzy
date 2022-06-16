#include "fuzzy.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <math.h>
#include <limits>

#define GET_DOUBLE_VALUE(JSON, KEY) ((JSON.begin().value().at(KEY).get<double>()))

Curve::Curve(
    double lower_bound, double upper_bound,
    bool lower_inclusive, bool upper_unclusive
):
_lower_bound(lower_bound), _upper_bound(upper_bound),
_upper_inclusive(upper_unclusive), _lower_inclusive(lower_inclusive) {}

Curve::Curve(const json &j)
{
    try
    {
        _lower_bound = j.begin().value().at("bounds").at("lower").get<double>();
    }
    catch(const json::out_of_range &e)
    {
        _lower_bound = - std::numeric_limits<double>::infinity();
    }
    try
    {
        _upper_bound = j.begin().value().at("bounds").at("upper").get<double>();
    }
    catch(const json::out_of_range &e)
    {
        _upper_bound = + std::numeric_limits<double>::infinity();
    }
    try
    {
        _upper_inclusive =
            j.begin().value().at("bounds").at("upper_inclusive").get<bool>();
    }
    catch(const json::out_of_range &e) {}
    try
    {
        _lower_inclusive =
            j.begin().value().at("bounds").at("lower_inclusive").get<bool>();
    }
    catch(const json::out_of_range &e) {}
}

double Curve::get_lower_bound(void) const {return _lower_bound;}

double Curve::get_upper_bound(void) const {return _upper_bound;}

void Curve::set_lower_bound(double value)
{
    if (value > _upper_bound)
    {
        _lower_bound = _upper_bound;
        _upper_bound = value;
    }
    else 
    {
        _lower_bound = value;
    }
}

void Curve::set_upper_bound(double value)
{
    if (value < _lower_bound)
    {
        _upper_bound = _lower_bound;
        _lower_bound = value;
    }
    else 
    {
        _upper_bound = value;
    }
}

bool Curve::contains(double value)
{
    bool result = false;
    if (_lower_inclusive) result = (_lower_bound <= value);
    else result = (_lower_bound < value);
    if (_upper_inclusive) result *= (value <= _upper_bound);
    else result *= (value < _upper_bound);
    return result;
}

bool Curve::is_finite(void)
{
    return isfinite(_lower_bound) && isfinite(_upper_bound);
}

ConstantCurve::ConstantCurve(void): Curve(), _value(0) {}

ConstantCurve::ConstantCurve(
    double lower_bound, double upper_bound, double value,
    bool lower_inclusive, bool upper_unclusive
): Curve(lower_bound, upper_bound, lower_inclusive, upper_unclusive),
_value(value) {}

ConstantCurve::ConstantCurve(const json &j): Curve(j)
{
    _value = GET_DOUBLE_VALUE(j, "value");
    // _value = j.begin().value().at("value").get<double>();
}

double ConstantCurve::membership(double input) {return _value;}

LinearCurve::LinearCurve(void): Curve(), _slope(0), _intercept(0) {}

LinearCurve::LinearCurve(
    double lower_bound, double upper_bound,
    double slope, double intercept,
    bool lower_inclusive, bool upper_unclusive
):Curve(lower_bound, upper_bound, lower_inclusive, upper_unclusive),
_slope(slope), _intercept(intercept) {}

LinearCurve::LinearCurve(const json &j): Curve(j)
{
    _slope = GET_DOUBLE_VALUE(j, "slope");
    _intercept = GET_DOUBLE_VALUE(j, "intercept");
    // _slope = j.begin().value().at("slope").get<double>();
    // _intercept = j.begin().value().at("intercept").get<double>();
}

double LinearCurve::membership(double input)
{
    return _slope * input + _intercept;
}

QuadraticCurve::QuadraticCurve(void) :Curve(), _a(0), _b(0), _c(0) {}

QuadraticCurve::QuadraticCurve(
    double lower_bound, double upper_bound,
    double a, double b, double c,
    bool lower_inclusive, bool upper_unclusive
): Curve(lower_bound, upper_bound, lower_inclusive, upper_unclusive),
_a(a), _b(b), _c(c) {}

QuadraticCurve::QuadraticCurve(const json &j): Curve(j)
{
    _a = GET_DOUBLE_VALUE(j, "a");
    _b = GET_DOUBLE_VALUE(j, "b");
    _c = GET_DOUBLE_VALUE(j, "c");
}

double QuadraticCurve::membership(double input)
{
    return _a*input*input + _b*input + _c;
}

LogarithmicCurve::LogarithmicCurve(void):
Curve(), _base(M_E), _x_offset(0), _y_offset(0) {}

LogarithmicCurve::LogarithmicCurve(
    double lower_bound, double upper_bound,
    double base, double x_offset, double y_offset,
    bool lower_inclusive, bool upper_unclusive
): Curve(lower_bound, upper_bound, lower_inclusive, upper_unclusive),
_base(base), _x_offset(x_offset), _y_offset(y_offset) {}

LogarithmicCurve::LogarithmicCurve(const json &j): Curve(j)
{
    _base = GET_DOUBLE_VALUE(j, "base");
    _x_offset = GET_DOUBLE_VALUE(j, "x_offset");
    _y_offset = GET_DOUBLE_VALUE(j, "y_offset");
}

double LogarithmicCurve::membership(double input)
{
    return log(input - _x_offset) / log(_base - _x_offset) + _y_offset;
}

ExponentialCurve::ExponentialCurve(void):
Curve(), _base(M_E), _x_offset(0), _y_offset(0) {}

ExponentialCurve::ExponentialCurve(
    double lower_bound, double upper_bound,
    double base, double x_offset, double y_offset,
    bool lower_inclusive, bool upper_unclusive
): Curve(lower_bound, upper_bound, lower_inclusive, upper_unclusive),
_base(base), _x_offset(x_offset), _y_offset(y_offset) {}

ExponentialCurve::ExponentialCurve(const json &j): Curve(j)
{
    _base = GET_DOUBLE_VALUE(j, "base");
    _x_offset = GET_DOUBLE_VALUE(j, "x_offset");
    _y_offset = GET_DOUBLE_VALUE(j, "y_offset");
}

double ExponentialCurve::membership(double input)
{
    return _base * (input - _x_offset) * (input - _x_offset) + _y_offset;
}

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
