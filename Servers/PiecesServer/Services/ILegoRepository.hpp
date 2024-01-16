//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_ILEGOREPOSITORY_HPP
#define HARBOR_CLS_ILEGOREPOSITORY_HPP

#include <vector>
#include <string>

#include "../LegoFigure.hpp"

class ILegoRepository {
public:
  virtual ~ILegoRepository() = default;

  virtual std::vector<std::string> getAllNames() = 0;

  virtual std::vector<LegoFigure>& getAllFigures() = 0;
};

#endif //HARBOR_CLS_ILEGOREPOSITORY_HPP
