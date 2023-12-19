//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_NOLOGRATATION_HPP
#define LEGO_FIGURE_MAKER_NOLOGRATATION_HPP

#include "ILogFileRotation.hpp"
#include "BaseFileRotation.hpp"

class NoLogFileRotation : public BaseFileRotation {
public:
    NoLogFileRotation() = default;

    ~NoLogFileRotation() override = default;

    /**
     * @brief Performs no operations regarding the lifetime of the log file.
     * @param file The file to rotate.
     * @param fileName The name of the file to rotate.
     */
    void rotateLogFile(std::ofstream& file, std::string& fileName) override {
      if (!file.is_open()) {
        std::string logFileName = NoLogFileRotation::createLogFileName(fileName, true);

        file.open(logFileName, std::ios::app);
      }
    }
};

#endif //LEGO_FIGURE_MAKER_NOLOGRATATION_HPP
