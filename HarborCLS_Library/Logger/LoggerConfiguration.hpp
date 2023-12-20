//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_LOGGERCONFIGURATION_HPP
#define HARBOR_CLS_LOGGERCONFIGURATION_HPP

namespace HarborCLS {

  struct LoggerConfiguration {
    enum class FileAlwaysOpenPolicy {
      DEFAULT = 0,
      ALWAYS_OPEN,
      OPEN_AND_CLOSE
    } fileAlwaysOpenPolicy{FileAlwaysOpenPolicy::ALWAYS_OPEN};

    enum class BufferingPolicy {
      DEFAULT = 0,
      NO_BUFFER
    } bufferingPolicy{BufferingPolicy::NO_BUFFER};

    enum class FileRotationPolicy {
      DEFAULT = 0,
      NO_ROTATION,
      BOUNDED_ROTATION
    } fileRotationPolicy{FileRotationPolicy::NO_ROTATION};

    bool sharedLog{true};

    std::string logFilePath{"log.txt"};

    int maxFileSize{100};

    bool operator==(const LoggerConfiguration &otherConf) const {
      return fileAlwaysOpenPolicy == otherConf.fileAlwaysOpenPolicy &&
             bufferingPolicy == otherConf.bufferingPolicy &&
             fileRotationPolicy == otherConf.fileRotationPolicy &&
             sharedLog == otherConf.sharedLog &&
             logFilePath == otherConf.logFilePath &&
             maxFileSize == otherConf.maxFileSize;
    }
  };
}

#endif //HARBOR_CLS_LOGGERCONFIGURATION_HPP
