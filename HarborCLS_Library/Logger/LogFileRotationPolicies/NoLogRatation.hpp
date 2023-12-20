//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_NOLOGRATATION_HPP
#define HARBOR_CLS_NOLOGRATATION_HPP

#include "ILogFileRotation.hpp"
#include "BaseFileRotation.hpp"

namespace HarborCLS {

  class NoLogFileRotation : public BaseFileRotation {
  public:
    NoLogFileRotation() = default;

    ~NoLogFileRotation() override = default;

    /**
     * @brief Performs no operations regarding the lifetime of the log file.
     * @param file The file to rotate.
     * @param fileName The name of the file to rotate.
     */
    void rotateLogFile(std::ofstream &file, std::string &fileName) override {
      if (!file.is_open()) {
        fileName = NoLogFileRotation::createLogFileName(fileName, true);

        file.open(fileName, std::ios::app);
      }
    }
  };
}

#endif //HARBOR_CLS_NOLOGRATATION_HPP
