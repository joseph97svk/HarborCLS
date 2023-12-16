//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEMANAGEMENT_HPP
#define LEGO_FIGURE_MAKER_FILEMANAGEMENT_HPP

#include <string>
#include <filesystem>

inline static std::string getAbsolutePath (std::string& relativePath) {
  std::filesystem::path currentPath = __FILE__;
  currentPath = currentPath.parent_path();

  std::string absolutePath = currentPath / relativePath;

  return absolutePath;
}

inline static std::string getPath(std::string& relativePath) {
  std::filesystem::path pathObj(relativePath);

  std::filesystem::path dirPath = pathObj.parent_path();

  return dirPath.string();
}

inline static bool pathExists(std::string& path) {
  std::filesystem::path pathObj(path);

  std::filesystem::path dirPath = pathObj.parent_path();

  return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

#endif //LEGO_FIGURE_MAKER_FILEMANAGEMENT_HPP
