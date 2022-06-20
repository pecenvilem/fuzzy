#include "curves.h"
#include "fuzzy.h"
#include "json.hpp"
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

json Curve::get_json(void)
{
    json j;
    if (isfinite(_upper_bound))
    {
        j["upper"] = _upper_bound;
        j["upper_inclusive"] = _upper_inclusive;
    }
    if (isfinite(_lower_bound))
    {
        j["lower"] = _lower_bound;
        j["lower_inclusive"] = _lower_inclusive;
    }

    return json{{"bounds", j}};
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

json ConstantCurve::get_json(void)
{
    json j = Curve::get_json();
    j["value"] = _value;
    return json{{"ConstantCurve", j}};
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

json LinearCurve::get_json(void)
{
    json j = Curve::get_json();
    j["slope"] = _slope;
    j["intercept"] = _intercept;
    return json{{"LinearCurve", j}};
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

json QuadraticCurve::get_json(void)
{
    json j = Curve::get_json();
    j["a"] = _a; j["b"] = _b; j["c"] = _c;
    return json{{"QuadraticCurve", j}};
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

json LogarithmicCurve::get_json(void)
{
    json j = Curve::get_json();
    j["base"] = _base; j["x_offset"] = _x_offset;
    j["y_offset"] = _y_offset;
    return json{{"LogarithmicCurve", j}};
}

double LogarithmicCurve::membership(double input)
{
    return log(input - _x_offset) / log(_base) + _y_offset;
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

json ExponentialCurve::get_json(void)
{
    json j = Curve::get_json();
    j["base"] = _base; j["x_offset"] = _x_offset;
    j["y_offset"] = _y_offset;
    return json{{"ExponentialCurve", j}};
}

double ExponentialCurve::membership(double input)
{
    return pow(_base, (input - _x_offset)) +  _y_offset;
}