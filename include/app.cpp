#include "app.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "json.hpp"
#include "curves.h"

void display(std::vector<std::string> choices)
{
    int current = 1;
    for (auto choice: choices)
    {
        std::cout << current++ << " > " << choice << std::endl;
    }
}

void App::loop(void)
{
    App_function_pointer command = _main_menu.back().command;
    while (_run)
    {
        system("CLS");
        try
        {
            command = _select(_main_menu);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception occured!" << std::endl;
            std::cerr << e.what() << std::endl;
            system("PAUSE");
            continue;
        }
        (this->*command)();
        if (_run) system("PAUSE");
    }
}

App::App_function_pointer App::_select(Menu menu)
{
    std::vector<std::string> choices;
    for (App::Option option: menu) choices.push_back(option.text);
    display(choices);
    int choice;
    std::cout << "Choose option:" << std::endl;
    std::cin >> choice;
    if (std::cin.fail())
        throw std::invalid_argument("Input choice unrecognized!");
    if (!(1 <= choice && choice <= choices.size()))
        throw std::invalid_argument("Chosen option out of range!");
    return menu[choice-1].command;
}

void App::_load_from_json(void)
{
    std::string file_name = ask_user<std::string>("Enter file name: ");
    try {_load_from_json(file_name);}
    catch (std::exception &e) {std::cout << e.what() << std::endl;}
}

void App::_load_from_json(std::string filename)
{
    std::ifstream input_file(filename);
    if (!input_file.good())
    {
        std::string message = "Failed to open file '" + filename + "'!";
        throw std::invalid_argument(message);
    }
    try
    {
        json input_json = json::parse(input_file);
        for (auto j: input_json) _sets.push_back(FuzzySet(j));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Loading from file '" + filename + "'failed!";
        std::cerr << e.what() << '\n';
    }
}

void App::_create_new(void)
{
    std::string name = ask_user<std::string>("Enter fuzzy set name: ");
    std::vector<std::string> choices;
    for (CurveParameters p: defined_curves()) choices.push_back(p.name);
    
    bool next_curve = true;
    bool enter_lower, enter_upper;
    bool lower_inclusive, upper_inclusive;
    double lower, upper;
    int choice;
    CurveParameters chosen_curve;
    json j_curve, j_set = json::array();
    while (next_curve)
    {
        j_curve.clear();
        display(choices);
        choice = ask_user<int>("Choose curve type: ") - 1;
        chosen_curve = defined_curves()[choice];
        if (ask_user<bool>("Define lower bound of curve? [0/1 >> No/Yes]: "))
        {
            j_curve["bounds"]["lower"] =
                ask_user<double>("Enter lower bound: ");
            j_curve["bounds"]["lower_inclusive"] =
                ask_user<bool>("Is lower bound inclusive? [0/1 >> No/Yes]: ");
        }

        if (ask_user<bool>("Define upper bound of curve? [0/1 >> No/Yes]: "))
        {
            j_curve["bounds"]["upper"] =
                ask_user<double>("Enter upper bound: ");
            j_curve["bounds"]["upper_inclusive"] =
                ask_user<bool>("Is upper bound inclusive? [0/1 >> No/Yes]: ");
        }

        for (std::string parameter: chosen_curve.parameters)
        {
            j_curve[parameter] =
                ask_user<double>("Enter parameter '" + parameter + "': ");
        }
        j_set.push_back(json {{chosen_curve.name, j_curve}});
        next_curve = ask_user<bool>("Next curve segment? [0/1 >> No/Yes]: ");
    }
    _sets.push_back(FuzzySet(json {{name, j_set}}));
    
}

void App::_evaluate(void)
{
    // double value;
    // std::cout << "Enter value to evaluate Fuzzy sets at: ";
    // std::cin >> value;
    double value = ask_user<double>(
        "Enter value to evaluate membership functions at"
    );
    _evaluate(value);
}

void App::_evaluate(double value)
{
    std::cout << "Evaluating membership functions for input value: "
        << value << std::endl;
    for (FuzzySet set: _sets)
        {
            std::cout << "Set: '" << set.get_name() <<
                "'\t membership: " << set.membership(value) << std::endl;
        }
}

void App::_delete(void)
{
    std::vector<std::string> names;
    for (FuzzySet set: _sets) names.push_back(set.get_name());
    display(names);
    int choice = ask_user<int>("Select fuzzy set for deletion: ");
    if (!(1 <= choice && choice <= _sets.size()))
        throw std::invalid_argument("Selected index out of range!");
    _sets.erase(_sets.begin() + choice);
}

void App::_save(void)
{
    try {_save(ask_user<std::string>("Enter name of the ouput file: "));}
    catch (std::exception &e) {std::cout << e.what() << std::endl;}
}

void App::_save(std::string filename)
{
    std::ofstream output_file(filename);
    if (!output_file.good())
    {
        std::string message = "Failed to open file '" + filename + "'!";
        throw std::invalid_argument(message);
    }
    json j = json::array();
    for (FuzzySet set: _sets) j.push_back(set.get_json());
    output_file << j.dump(4) << std::endl;
}

void App::_plot(void)
{
    std::string command_invocation = "CALL plot\\plot.bat";
    system("mkdir tmp");
    for (FuzzySet set: _sets)
    {
        std::string filename = "tmp\\" + set.get_name() + ".csv";
        set.generate_plot_data(filename);
        command_invocation += " " + filename;
    }
    system(command_invocation.c_str());
    system("rmdir tmp /s /q");
}

void App::_export_csv(void)
{
    for (FuzzySet set: _sets)
    set.generate_plot_data("output\\" + set.get_name() + ".csv");
}

void App::_install_plotting(void)
{
    system("CALL plot\\install.bat");
}

void App::_uninstall_plotting(void)
{
    system("CALL plot\\uninstall.bat");
}


