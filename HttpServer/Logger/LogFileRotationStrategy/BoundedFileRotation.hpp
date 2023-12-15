//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP
#define LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP

#include "ILogFileRotation.hpp"
#include "../../common.hpp"

class BoundedFileRotation : public ILogFileRotation {
    unsigned int fileMaxWrites {};
    unsigned int fileCurrentWrites {};

    constexpr static const char* LOG_FILE_EXTENSION = ".log";
    constexpr static const char* LOG_FILE_POSTFIX = "_log_file_on_";

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

        size_t extensionBegin = fileName.find_last_of('.');

        std::string newFileName =
                (extensionBegin == std::string::npos) ?
                fileName : fileName.substr(0, extensionBegin);

        if (newFileName.find(LOG_FILE_POSTFIX) != std::string::npos) {
          newFileName = newFileName.substr(0, newFileName.find(LOG_FILE_POSTFIX));
        }

        std::string currentTimeAndDate = getCurrentTimeAndDate();
        newFileName += LOG_FILE_POSTFIX + currentTimeAndDate + LOG_FILE_EXTENSION;

        file.open(newFileName, std::ios::out | std::ios::trunc);
      }

      fileCurrentWrites++;
    }
};

#endif //LEGO_FIGURE_MAKER_BOUNDEDFILEROTATION_HPP
