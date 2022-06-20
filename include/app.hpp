#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "fuzzy.h"

template <class T>
T ask_user(std::string prompt)
{
    T result;
    std::cout << prompt;
    std::cin.clear();
    std::cin >> result;
    if (std::cin.fail())
        throw std::invalid_argument("Failed to convert input!");
    return result;
}

void display(std::vector<std::string> choices);

class App
{
    public:
        void loop(void);
    private:
        typedef void (App::*App_function_pointer)(void);
        
        typedef struct option
        {
            std::string text;
            App_function_pointer command;
        } Option;
        
        typedef std::vector<Option> Menu;

        App_function_pointer _select(Menu menu);

        void _load_from_json(void);
        void _load_from_json(std::string filename);
        void _create_new(void);
        void _evaluate(void);
        void _evaluate(double value);
        void _delete(void);
        void _save(void);
        void _save(std::string filename);
        void _plot(void);
        void _export_csv(void);
        void _install_plotting(void);
        void _uninstall_plotting(void);
        void _terminate(void) {_run = false;}

    private:
        bool _run = true;
        std::vector<FuzzySet> _sets;
        Menu _main_menu =
        {
            {"Load fuzzy set from JSON file", &App::_load_from_json},
            {"Create new fuzzy set", &App::_create_new},
            {"Delete a loaded fuzzy set", &App::_delete},
            {"Save fuzzy sets to JSON file", &App::_save},
            {"Evaluate membership functions at given point", &App::_evaluate},
            {"Sample membership functions into CSV files", &App::_export_csv},
            {"Plot all membership functions", &App::_plot},
            {"Install plotting module", &App::_install_plotting},
            {"Uninstall plotting module", &App::_uninstall_plotting},
            {"Exit program", &App::_terminate}
        };
};