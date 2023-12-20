//
// Created by josephvalverde on 12/18/23.
//

#ifndef LEGO_FIGURE_MAKER_PATHMANAGER_HPP
#define LEGO_FIGURE_MAKER_PATHMANAGER_HPP

#include <filesystem>
#include <iostream>

#define defineDirectory() setDirectory();

static void ensureLogsDirectoryExists();

inline static void setDirectory() {
  std::string currentPath = __FILE__;

  while (currentPath.contains("HttpServer")) {
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

#endif //LEGO_FIGURE_MAKER_PATHMANAGER_HPP
