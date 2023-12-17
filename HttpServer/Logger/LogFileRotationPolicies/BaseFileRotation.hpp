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
  constexpr static const char* LOG_FILE_EXTENSION = ".log";
  constexpr static const char* LOG_FILE_POSTFIX = "_logging_on_";
public:
  BaseFileRotation() = default;

  ~BaseFileRotation() override = default;

  /**
   * @brief Performs the implementation lifetime behavior of the log file rotation policy.
   * @param file The file to rotate.
   * @param fileName The name of the file to rotate.
   */
  void rotateLogFile(std::ofstream& file, std::string& fileName) override = 0;

  /**
   * @brief Creates a new log file name based on the given file name.
   * @param fileName original file name
   * @param completeTimeAndDate whether to include the time and date in the new file name
   * @return the new file name
   */
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
