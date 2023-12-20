//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_ILOGFILEROTATION_HPP
#define HARBOR_CLS_ILOGFILEROTATION_HPP

namespace HarborCLS {

  class ILogFileRotation {
  public:
    virtual ~ILogFileRotation() = default;

    /**
     * @brief Performs the implementation lifetime behavior of the log file rotation policy.
     * @param file The file to rotate.
     * @param fileName The name of the file to rotate.
     */
    virtual void rotateLogFile(std::ofstream &file, std::string &fileName) = 0;
  };
}

#endif //HARBOR_CLS_ILOGFILEROTATION_HPP
