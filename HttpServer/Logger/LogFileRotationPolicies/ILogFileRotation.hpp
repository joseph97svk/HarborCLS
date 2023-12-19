//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGFILEROTATION_HPP
#define LEGO_FIGURE_MAKER_ILOGFILEROTATION_HPP

class ILogFileRotation {
public:
  virtual ~ILogFileRotation() = default;

  /**
   * @brief Performs the implementation lifetime behavior of the log file rotation policy.
   * @param file The file to rotate.
   * @param fileName The name of the file to rotate.
   */
  virtual void rotateLogFile(std::ofstream& file, std::string& fileName) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGFILEROTATION_HPP
