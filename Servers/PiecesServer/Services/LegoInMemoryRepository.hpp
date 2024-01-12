//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP
#define HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP

#include <vector>

#include "ILegoRepository.hpp"

class LegoInMemoryRepository : public ILegoRepository {
  std::vector<LegoFigure> _legoFigures;

  public:
    LegoInMemoryRepository();

    std::vector<std::string> getAllNames() override;

    std::vector<LegoFigure>& getAllFigures() override;
};

#endif //HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP
