//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_PATHMANAGER_HPP
#define HARBOR_CLS_PATHMANAGER_HPP

#include <filesystem>
#include <iostream>

#define defineDirectory() setDirectory();

namespace HarborCLS {

  static void ensureLogsDirectoryExists();

  inline static void setDirectory() {
    std::string currentPath = __FILE__;

    while (currentPath.contains("HarborCLS_Library")) {
      currentPath = currentPath.substr(0, currentPath.find_last_of('/'));
    }

    if (currentPath != std::filesystem::current_path()) {
      std::filesystem::current_path(currentPath);
    }

    ensureLogsDirectoryExists();
  }

  inline static void ensureLogsDirectoryExists() {
    std::filesystem::path logsPath = std::filesystem::current_path() / "Logs";

    if (!std::filesystem::exists(logsPath)) {
      std::filesystem::create_directory(logsPath);
    }
  }
}

#endif //HARBOR_CLS_PATHMANAGER_HPP
