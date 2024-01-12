//
// Created by josephvalverde on 1/11/24.
//

#include <fstream>
#include <optional>
#include "LegoInMemoryRepository.hpp"

std::optional<LegoFigure> readLegoFigure(std::ifstream& file);

std::optional<LegoPiece> readLegoPiece(std::ifstream& file);

std::vector<std::string> LegoInMemoryRepository::getAllNames() {
  std::vector<std::string> names;
  for (auto& figure : _legoFigures) {
    names.push_back(figure.figureName);
  }
  return names;
}

std::vector<LegoFigure>& LegoInMemoryRepository::getAllFigures() {
  return _legoFigures;
}

LegoInMemoryRepository::LegoInMemoryRepository() {
  std::string legoFilePath = "Servers/PiecesServer/LegoFile.txt";

  std::string extension = legoFilePath.substr(legoFilePath.find_last_of(".") + 1);

  if (extension != "txt") {
    throw std::runtime_error("Lego file must be a .txt file");
  }

  std::ifstream legoFile(legoFilePath);

  if (!legoFile.is_open()) {
    throw std::runtime_error("Lego file could not be opened");
  }

  std::string line;

  std::getline(legoFile, line);

  if (line != "Lego source File :: group ESJOJO") {
    throw std::runtime_error("Lego file is not compatible with \"Lego source File :: group ESJOJO\"");
  }

  while (legoFile) {
    std::optional<LegoFigure> figure = readLegoFigure(legoFile);
    if (figure.has_value()) {
      _legoFigures.push_back(figure.value());
    }
  }
}


std::optional<LegoFigure> readLegoFigure(std::ifstream& file) {
  std::string line;

  std::string legoFigureName;
  std::string legoFigureImage;

  std::getline(file, line);

  if (line.empty()) {
    return std::nullopt;
  }

  legoFigureName = line;

  std::getline(file, line);

  if (line.empty()) {
    return std::nullopt;
  }

  legoFigureImage = line;

  LegoFigure legoFigure {
    .figureName = legoFigureName,
    .imagePath = legoFigureImage
  };

  std::optional<LegoPiece> legoPiece;

  do {
    legoPiece = readLegoPiece(file);
    if (legoPiece.has_value()) {
      legoFigure.legoPieces.push_back(legoPiece.value());
    }
  } while (legoPiece.has_value());

  return legoFigure;
}

std::optional<LegoPiece> readLegoPiece(std::ifstream& file) {
  std::string line;

  std::getline(file, line);
  if (line == "*") {
    return std::nullopt;
  }
  std::string legoPieceName = line;

  std::getline(file, line);
  if (line.empty()) {
    return std::nullopt;
  }
  std::string legoPieceImage = line;

  std::getline(file, line);
  if (line.empty()) {
    return std::nullopt;
  }
  size_t legoPieceAmount = std::stoull(line);

  return LegoPiece {
    .pieceName = legoPieceName,
    .imagePath = legoPieceImage,
    .quantity = legoPieceAmount
  };
}