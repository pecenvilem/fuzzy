#ifndef __FUZZY__H__
#define __FUZZY__H__

#include "json.hpp"
#include <map>
#include <fstream>
#include <vector>
#include <string>

using json = nlohmann::json;

class Curve
{
    /* Abstract base class representing one individual segment of
    membership funcion of Fuzzy Set */
private:
    double _lower_bound = 0, _upper_bound = 0;
    bool _lower_inclusive = false, _upper_inclusive = false;
public:
    Curve(void) {};
    Curve(
        double lower_bound, double upper_bound,
        bool lower_inclusive = true, bool upper_unclusive = true
    );
    Curve(const json &j);

    double get_lower_bound(void) const;
    double get_upper_bound(void) const;
    void set_lower_bound(double value);
    void set_upper_bound(double value);

    bool contains(double value);
    bool is_finite(void);

    virtual double membership(double input) = 0;
};

class ConstantCurve: public Curve
{
    // y = value
    private:
        double _value;
    public:
        ConstantCurve(void);
        ConstantCurve(
            double lower_bound, double upper_bound, double value,
            bool lower_inclusive = true, bool upper_unclusive = true
        );
        ConstantCurve(const json &j);
        double membership(double input) override;
};

class LinearCurve: public Curve
{
    // y = slope * x + intercept
    private:
        double _slope, _intercept;
    public:
        LinearCurve(void);
        LinearCurve(
            double lower_bound, double upper_bound,
            double slope, double intercept,
            bool lower_inclusive = true, bool upper_unclusive = true
        );
        LinearCurve(const json &j);
        double membership(double input) override;
};

class QuadraticCurve: public Curve
{
    // y = a*x^2 + b*x + c
    private:
        double _a;      // quadratic coefficient
        double _b;      // linear coefficient
        double _c;      // absolute coefficient
    public:
        QuadraticCurve(void);
        QuadraticCurve(
            double lower_bound, double upper_bound,
            double a, double b, double c,
            bool lower_inclusive = true, bool upper_unclusive = true
        );
        QuadraticCurve(const json &j);
        double membership(double input) override;
};

class LogarithmicCurve: public Curve
{
    // y = log_'base'(x + 'x_offset') + 'y_offset'
    private:
        double _base;
        double _x_offset, _y_offset;
    public:
        LogarithmicCurve(void);
        LogarithmicCurve(
            double lower_bound, double upper_bound,
            double base, double x_offset, double y_offset,
            bool lower_inclusive = true, bool upper_unclusive = true
        );
        LogarithmicCurve(const json &j);
        double membership(double input) override;
};

class ExponentialCurve: public Curve
{
    // y = 'base'^(x + 'x_offset') + 'y_offset'
    private:
        double _base;
        double _x_offset, _y_offset;
    public:
        ExponentialCurve(void);
        ExponentialCurve(
            double lower_bound, double upper_bound,
            double base, double x_offset, double y_offset,
            bool lower_inclusive = true, bool upper_unclusive = true
        );
        ExponentialCurve(const json &j);
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


#endif  //!__FUZZY__H__