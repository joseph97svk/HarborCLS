//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BASEFILEROTATION_HPP
#define LEGO_FIGURE_MAKER_BASEFILEROTATION_HPP

#include <fstream>
#include <string>
#include "ILogFileRotation.hpp"
#include "../../common.hpp"

class BaseFileRotation : public ILogFileRotation {
protected:
  constexpr static const char* LOG_FILE_EXTENSION = ".txt";
  constexpr static const char* LOG_FILE_POSTFIX = "_logging_on_";
public:
  BaseFileRotation() = default;

  ~BaseFileRotation() override = default;

  void rotateLogFile(std::ofstream& file, std::string& fileName) override = 0;

  static std::string createLogFileName(std::string& fileName, bool completeTimeAndDate) {
    size_t extensionBegin = fileName.find_last_of('.');

    std::string newFileName =
            (extensionBegin == std::string::npos) ?
            fileName : fileName.substr(0, extensionBegin);

    if (newFileName.find(LOG_FILE_POSTFIX) != std::string::npos) {
      newFileName = newFileName.substr(0, newFileName.find(LOG_FILE_POSTFIX));
    }

    std::string currentTimeAndDate = completeTimeAndDate? getCurrentTimeAndDate() : getCurrentDate();
    newFileName += LOG_FILE_POSTFIX + currentTimeAndDate + LOG_FILE_EXTENSION;

    return newFileName;
  }
};

#endif //LEGO_FIGURE_MAKER_BASEFILEROTATION_HPP
