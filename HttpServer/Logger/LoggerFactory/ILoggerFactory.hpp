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

    /**
     * @brief Create a shared logger.
     * @return A shared logger.
     */
    virtual std::shared_ptr<ILogger> createLogger() = 0;

    /**
     * @brief Create a unique logger.
     * @return A unique logger.
     */
    virtual std::unique_ptr<ILogger> createUniqueLogger() = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERFACTORY_HPP
