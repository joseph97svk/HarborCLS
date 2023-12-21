//
// Created by josephvalverde on 12/14/23.
//

#ifndef HARBOR_CLS_FILEMANAGEMENT_HPP
#define HARBOR_CLS_FILEMANAGEMENT_HPP

#include <string>
#include <filesystem>

namespace HarborCLS {

/**
 * @brief Returns the absolute path of the file.
 * @param relativePath The relative path of the file.
 * @return The absolute path of the file.
 */
  inline static std::string getAbsolutePath(std::string &relativePath) {
    std::filesystem::path currentPath = __FILE__;
    currentPath = currentPath.parent_path();

    std::string absolutePath = currentPath / relativePath;

    return absolutePath;
  }

/**
 * @brief Returns the path of the file.
 * @param relativePath The relative path of the file.
 * @return The path of the file.
 */
  inline static std::string getPath(std::string &relativePath) {
    std::filesystem::path pathObj(relativePath);

    std::filesystem::path dirPath = pathObj.parent_path();

    return dirPath.string();
  }

/**
 * @brief Returns the name of the file.
 * @param relativePath The relative path of the file.
 * @return The name of the file.
 */
  inline static bool pathExists(std::string &path) {
    std::filesystem::path pathObj(path);

    std::filesystem::path dirPath = pathObj.parent_path();

    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
  }
}

#endif //HARBOR_CLS_FILEMANAGEMENT_HPP
