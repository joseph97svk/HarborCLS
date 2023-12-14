//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGER_HPP
#define LEGO_FIGURE_MAKER_ILOGGER_HPP

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(std::string& message) = 0;
    virtual void warning(std::string& message) = 0;
    virtual void error(std::string& message) = 0;
    virtual void info(std::string& message) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGER_HPP
