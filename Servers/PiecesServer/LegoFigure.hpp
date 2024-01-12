//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_LEGOFIGURE_HPP
#define HARBOR_CLS_LEGOFIGURE_HPP

#include <vector>
#include <string>

#include "LegoPiece.hpp"

struct LegoFigure {
  std::string figureName;

  std::string imagePath;

  std::vector<LegoPiece> legoPieces;
};

#endif //HARBOR_CLS_LEGOFIGURE_HPP
