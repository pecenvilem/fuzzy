#include "fuzzy.h"
#include "json.hpp"

using json = nlohmann::json;

Curve::Curve(void) {_lower_bound = 0; _upper_bound = 0;}

Curve::Curve(double lower_bound, double upper_bound)
{
    _lower_bound = lower_bound; _upper_bound = upper_bound;
}

Curve::Curve(const json &j)
{
    _lower_bound = j.at("bounds").at("lower").get<double>();
    _upper_bound = j.at("bounds").at("upper").get<double>();
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
    _value = j.at("value").get<double>();
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
    _slope = j.at("slope").get<double>();
    _intercept = j.at("intercept").get<double>();
}

double LinearCurve::membership(double input) {return _slope * input + _intercept;}

void to_json(json &j, const LinearCurve &c)
{
    json bounds{
            {"lower", c.get_lower_bound()},
            {"upper", c.get_upper_bound()}
    };
    j["bounds"] = bounds;
    j["slope"] = c._slope;
    j["intercept"] = c._intercept;
}

void from_json(const json &j, LinearCurve &c)
{   
    double lower_bound = j.at("bounds").at("lower").get<double>();
    double upper_bound = j.at("bounds").at("upper").get<double>();
    c = LinearCurve(
        lower_bound, upper_bound,
        j.at("slope").get<double>(),
        j.at("intercept").get<double>()
    );
}