//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_LOGGERCONFIGURATION_HPP
#define LEGO_FIGURE_MAKER_LOGGERCONFIGURATION_HPP

struct LoggerConfiguration {
  enum class FileAlwaysOpenPolicy{
    DEFAULT = 0,
    ALWAYS_OPEN,
    OPEN_AND_CLOSE
  } fileAlwaysOpenPolicy;

  enum class BufferingPolicy {
    DEFAULT = 0,
    NO_BUFFER
  } bufferingPolicy;

  std::string logFilePath;
};

#endif //LEGO_FIGURE_MAKER_LOGGERCONFIGURATION_HPP
