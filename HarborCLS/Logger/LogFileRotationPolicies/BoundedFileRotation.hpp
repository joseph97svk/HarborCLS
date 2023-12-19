//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_BOUNDEDFILEROTATION_HPP
#define HARBOR_CLS_BOUNDEDFILEROTATION_HPP

#include "ILogFileRotation.hpp"
#include "BaseFileRotation.hpp"
#include "Common/common.hpp"

namespace HarborCLS {

  class BoundedFileRotation : public BaseFileRotation {
    unsigned int _fileMaxWrites{};
    unsigned int _fileCurrentWrites{};

  public:
    explicit BoundedFileRotation(unsigned int fileMaxWrites)
        : _fileMaxWrites(fileMaxWrites) {}

    ~BoundedFileRotation() override = default;

    /**
     * @brief Checks if the amount of writes to the file has exceeded the maximum amount of writes. If so, the file is
     * closed and a new file is opened.
     * @param file The file to rotate.
     * @param fileName The name of the file to rotate.
     */
    void rotateLogFile(std::ofstream &file, std::string &fileName) override {
      if (_fileCurrentWrites >= _fileMaxWrites || _fileCurrentWrites == 0) {
        _fileCurrentWrites = 0;

        if (file.is_open()) {
          file.close();
        }

        fileName = BaseFileRotation::createLogFileName(fileName, true);

        file.open(fileName, std::ios::app);
      }

      _fileCurrentWrites++;

      if (!file.is_open()) {
        file.open(fileName, std::ios::app);
      }
    }
  };
}

#endif //HARBOR_CLS_BOUNDEDFILEROTATION_HPP
