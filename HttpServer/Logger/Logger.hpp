//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_LOGGER_HPP
#define LEGO_FIGURE_MAKER_LOGGER_HPP

#include <string>
#include "ILogger.hpp"

class Logger : public ILogger {
public:
    Logger() = default;
    ~Logger() override = default;

    void log(std::string& message) override {

    }

    void warning(std::string& message) override {

    }

    void error(std::string& message) override {

    }

    void info(std::string& message) override {

    }
};


#endif //LEGO_FIGURE_MAKER_LOGGER_HPP
