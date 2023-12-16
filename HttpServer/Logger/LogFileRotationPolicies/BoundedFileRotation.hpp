//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP
#define LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP

#include "ILogFileRotation.hpp"
#include "BaseFileRotation.hpp"
#include "../../common.hpp"

class BoundedFileRotation : public BaseFileRotation {
    unsigned int fileMaxWrites {};
    unsigned int fileCurrentWrites {};

public:
    explicit BoundedFileRotation(unsigned int fileMaxWrites)
        : fileMaxWrites(fileMaxWrites) { }

    ~BoundedFileRotation() override = default;

    void rotateLogFile(std::ofstream& file, std::string& fileName) override {
      if (fileCurrentWrites >= fileMaxWrites) {
        fileCurrentWrites = 0;

        if (file.is_open()) {
          file.close();
        }

        std::string newFileName = BaseFileRotation::createLogFileName(fileName, true);

        file.open(newFileName, std::ios::app);
      }

      fileCurrentWrites++;
    }
};

#endif //LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP
