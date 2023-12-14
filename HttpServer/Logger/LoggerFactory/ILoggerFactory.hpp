//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGERFACTORY_HPP
#define LEGO_FIGURE_MAKER_ILOGGERFACTORY_HPP

#include <memory>

#include "../ILogger.hpp"

class ILoggerFactory {
public:
    virtual ~ILoggerFactory() = default;

    virtual std::shared_ptr<ILogger> createLogger() = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERFACTORY_HPP
