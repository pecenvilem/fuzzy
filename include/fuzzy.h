#ifndef __FUZZY__H__
#define __FUZZY__H__

#include "json.hpp"
#include <map>
#include <vector>
#include <string>

using json = nlohmann::json;

class Curve
{
    /* Abstract base class representing one individual segment of
    membership funcion of Fuzzy Set */
    friend void to_json(json &j, const Curve &c);
private:
    double _lower_bound, _upper_bound;
public:
    Curve(void);
    Curve(double lower_bound, double upper_bound);
    Curve(const json &j);

    double get_lower_bound(void) const;
    double get_upper_bound(void) const;
    void set_lower_bound(double value);
    void set_upper_bound(double value);

    virtual double membership(double input) = 0;
};

class ConstantCurve: public Curve
{
        friend void to_json(json &j, const ConstantCurve &c);
        friend void from_json(const json &j, ConstantCurve &c);
    private:
        double _value;
    public:
        ConstantCurve(void);
        ConstantCurve(
            double lower_bound, double upper_bound, double value
        );
        ConstantCurve(const json &j);
        double membership(double input) override;
};

class LinearCurve: public Curve
{
        friend void to_json(json &j, const LinearCurve &c);
        friend void from_json(const json &j, LinearCurve &c);
    private:
        double _slope, _intercept;
    public:
        LinearCurve(void);
        LinearCurve(
            double lower_bound, double upper_bound,
            double slope, double intercept
        );
        LinearCurve(const json &j);
        double membership(double input) override;
};


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
};

// void from_json(const json &j, std::vector<Curve*> curves);

// typedef Curve *(*CurveFactory)(const json &j);
// typedef const std::map<std::string, CurveFactory> CurveTypeResolverMap;

// template<typename T> Curve *createInstance(const json &j)
// {
//     return j.get<T>();
// }

// typedef std::map<std::string, Curve*(*)()> CurveTypeResolverMap;

// CurveTypeResolverMap map;
// map["LinearCurve"] = &createInstance<LinearCurve>;



template<typename T>
Curve * createInstance(json &j)
{
    return new T;
}

typedef Curve*(*CurveFactory)(json &j);

typedef std::map<std::string, CurveFactory> map_type;



#endif  //!__FUZZY__H__