//
// Created by josephvalverde on 1/17/24.
//

#ifndef HARBOR_CLS_STATICIMAGEPROVIDERSERVICE_HPP
#define HARBOR_CLS_STATICIMAGEPROVIDERSERVICE_HPP

#include <string>
#include <vector>
#include <fstream>

#include "Server/WebApplication/DependencyManagement/BaseWebAppService.hpp"

namespace HarborCLS {
  class StaticImageProviderService {

  public:
    std::optional<std::vector<char>> getImage(std::string& path) {
      if (path.empty()) {
        return std::nullopt;
      }

      std::fstream imageFile(path, std::ios::in | std::ios::binary);

      if (!imageFile.is_open()) {
        return std::nullopt;
      }

      std::vector<char> image ((std::istreambuf_iterator<char>(imageFile)),
                               (std::istreambuf_iterator<char>()));

      return image;
    }
  };
}




#endif //HARBOR_CLS_STATICIMAGEPROVIDERSERVICE_HPP
