//
// Created by josephvalverde on 12/18/23.
//

#ifndef LEGO_FIGURE_MAKER_PATHMANAGER_HPP
#define LEGO_FIGURE_MAKER_PATHMANAGER_HPP

namespace HarborCLS {
  class PathManager {
  protected:
    PathManager() = default;

  public:
    [[nodiscard]] static PathManager& getInstance() {
      static PathManager instance;
      return instance;
    }


  };
}

#endif //LEGO_FIGURE_MAKER_PATHMANAGER_HPP
