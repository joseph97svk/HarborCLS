//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP
#define LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP

#include "ILogFileRotation.hpp"
#include "BaseFileRotation.hpp"
#include "Common/common.hpp"

class BoundedFileRotation : public BaseFileRotation {
    unsigned int fileMaxWrites {};
    unsigned int fileCurrentWrites {};

public:
    explicit BoundedFileRotation(unsigned int fileMaxWrites)
        : fileMaxWrites(fileMaxWrites) { }

    ~BoundedFileRotation() override = default;

    /**
     * @brief Checks if the amount of writes to the file has exceeded the maximum amount of writes. If so, the file is
     * closed and a new file is opened.
     * @param file The file to rotate.
     * @param fileName The name of the file to rotate.
     */
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
