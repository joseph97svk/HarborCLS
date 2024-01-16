//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP
#define HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP

#include <vector>
#include <map>

#include "ILegoRepository.hpp"

class LegoInMemoryRepository : public ILegoRepository {
  std::map<std::string, LegoFigure> _legoFigureMap;

public:
  LegoInMemoryRepository();

  ~LegoInMemoryRepository() override = default;

  std::vector<std::string> getAllNames() override;

  std::vector<LegoFigure> getAllFigures() override;

  std::optional<std::reference_wrapper<LegoFigure>> getFigureByName(const std::string& name) override;
};

#endif //HARBOR_CLS_LEGOINMEMORYREPOSITORY_HPP
