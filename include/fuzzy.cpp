#include "fuzzy.h"
#include "json.hpp"
#include <iostream>
#include <map>
#include <cmath>
#include <math.h>

using json = nlohmann::json;

Curve::Curve(void) {_lower_bound = 0; _upper_bound = 0;}

Curve::Curve(double lower_bound, double upper_bound)
{
    _lower_bound = lower_bound; _upper_bound = upper_bound;
}

Curve::Curve(const json &j)
{
    std::cout << j.begin().value() << std::endl;

    try
    {
        _lower_bound = j.begin().value().at("bounds").at("lower").get<double>();
    }
    catch(const std::exception &e)
    {
        // std::cerr << e.what() << '\n';
    }
    try
    {
        _upper_bound = j.begin().value().at("bounds").at("upper").get<double>();
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // _lower_bound = j.at("bounds").at("lower").get<double>();
    // _upper_bound = j.at("bounds").at("upper").get<double>();
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
    std::cout << j.dump(4) << std::endl;
    _value = j.begin().value().at("value").get<double>();
}

double ConstantCurve::membership(double input) {return _value;}

void from_json(const json &j, ConstantCurve &c)
{
    double lower_bound;
    double upper_bound;

    std::cout << j;

    try
    {
        lower_bound = j.at("bounds").at("lower").get<double>();
    }
    catch(const std::exception& e)
    {
        lower_bound = -INFINITY;
    }
    try
    {
        upper_bound = j.at("bounds").at("upper").get<double>();
    }
    catch(const std::exception& e)
    {
        upper_bound = INFINITY;
    }
    
    c = ConstantCurve(
        lower_bound, upper_bound, j.at("value").get<double>()
    );
}

void to_json(json &j, const ConstantCurve &c)
{
    json bounds;
    if (isfinite(c.get_lower_bound())) bounds["lower"] = c.get_lower_bound();
    if (isfinite(c.get_upper_bound())) bounds["lower"] = c.get_upper_bound();
    j["bounds"] = bounds;
    j["value"] = c._value;
}

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
    
    double lower_bound;
    double upper_bound;

    try
    {
        lower_bound = j.at("bounds").at("lower").get<double>();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    try
    {
        upper_bound = j.at("bounds").at("upper").get<double>();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    c = LinearCurve(
        lower_bound, upper_bound,
        j.at("slope").get<double>(),
        j.at("intercept").get<double>()
    );
}

FuzzySet::FuzzySet(const std::string name, const std::vector<Curve*> curves)
{
    _name = name; _curves = curves;
}

FuzzySet::FuzzySet(const json &j)
{
    static const map_type map = {
        {"ConstantCurve", createInstance<ConstantCurve>},
        {"LinearCurve", createInstance<LinearCurve>}
    };

    // auto element = j.begin();
    // _name = element.key();
    // for ( auto j_curve : element.value() )
    // {
    //     std::cout << element.value();
    //     // std::string typr_str = j_curve.key();
    //     // _curves.push_back()
    // }
    _name = j.begin().key();
    for (auto element: j.begin().value())
    {
        std::cout << element << std::endl;
        std::cout << element.begin().key() << std::endl;
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

double FuzzySet::membership(double value) {return _curves[1]->membership(value);}

void from_json(const json &j, std::vector<Curve*> curves)
{
    for (auto element: j)
    {
        std::cout << element << std::endl;
        std::cout << element.begin().key() << std::endl;
        if (element.begin().key() == "ConstantCurve")
        {
            curves.push_back(new ConstantCurve(element));
        }
        else if (element.begin().key() == "LinearCurve")
        {
            curves.push_back(new LinearCurve(element));
        }
        
    }
    
}

void from_json(const json &j, FuzzySet &set)
{
    auto element = j.begin();
    set._name = element.key();
    std::cout << "Stripped set NAME:" << std::endl;
    std::cout << element.value().dump(4) << std::endl;
    set._curves = element.value().get<std::vector<Curve*>>();
}

void to_json(json &j, const FuzzySet &set)
{
    
}