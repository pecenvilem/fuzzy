#include "fuzzy.h"
#include "json.hpp"
#include <iostream>
#include <map>
#include <cmath>
#include <math.h>
#include <limits>

using json = nlohmann::json;

Curve::Curve(void) {_lower_bound = 0; _upper_bound = 0;}

Curve::Curve(double lower_bound, double upper_bound)
{
    _lower_bound = lower_bound; _upper_bound = upper_bound;
}

Curve::Curve(const json &j)
{
    try
    {
        _lower_bound = j.begin().value().at("bounds").at("lower").get<double>();
    }
    catch(const std::exception &e)
    {
        _lower_bound = - std::numeric_limits<double>::infinity();
    }
    try
    {
        _upper_bound = j.begin().value().at("bounds").at("upper").get<double>();
    }
    catch(const std::exception &e)
    {
        _upper_bound = + std::numeric_limits<double>::infinity();
    }
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

ConstantCurve::ConstantCurve(void): Curve() {_value = 0;}

ConstantCurve::ConstantCurve(
    double lower_bound, double upper_bound,
    double value
): Curve(lower_bound, upper_bound)
{
    _value = value;
}

ConstantCurve::ConstantCurve(const json &j): Curve(j)
{
    _value = j.begin().value().at("value").get<double>();
}

double ConstantCurve::membership(double input) {return _value;}

LinearCurve::LinearCurve(void): Curve() {_slope = 0; _intercept = 0;}

LinearCurve::LinearCurve(
    double lower_bound, double upper_bound,
    double slope, double intercept
): Curve(lower_bound, upper_bound)
{
    _slope = slope; _intercept = intercept;
}

LinearCurve::LinearCurve(const json &j): Curve(j)
{
    _slope = j.begin().value().at("slope").get<double>();
    _intercept = j.begin().value().at("intercept").get<double>();
}

double LinearCurve::membership(double input) {return _slope * input + _intercept;}

FuzzySet::FuzzySet(const std::string name, const std::vector<Curve*> curves)
{
    _name = name; _curves = curves;
}

FuzzySet::FuzzySet(const json &j)
{
    _name = j.begin().key();
    for (auto element: j.begin().value())
    {
        if (element.begin().key() == "ConstantCurve")
        {
            _curves.push_back(new ConstantCurve(element));
        }
        else if (element.begin().key() == "LinearCurve")
        {
            _curves.push_back(new LinearCurve(element));
        }
        
    }
}

FuzzySet::FuzzySet(const std::string name, const json &j_curves)
{
    _name = name;
}

FuzzySet::~FuzzySet(void)
{
    for (Curve *c : _curves) delete c;
}


// PLACEHOLDER!
double FuzzySet::membership(double value)
{
    return _curves[1]->membership(value);
}
// PLACEHOLDER!
