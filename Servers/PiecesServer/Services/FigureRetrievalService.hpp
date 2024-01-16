//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_FIGURERETRIEVALSERVICE_HPP
#define HARBOR_CLS_FIGURERETRIEVALSERVICE_HPP

#include <HarborCLS.hpp>
#include <utility>
#include "Protocol/LegoFigureMakerProtocol.hpp"

class FigureRetrievalService : public HarborCLS::BaseWebAppService<LegoFigureMakerProtocol> {
  using ResponseType = typename LegoFigureMakerProtocol::ResponseType;
  using RequestType = typename LegoFigureMakerProtocol::RequestType;

  std::shared_ptr<ILegoRepository> _legoRepository {};

public:
  explicit FigureRetrievalService(std::shared_ptr<ILegoRepository> legoRepository)
      : HarborCLS::BaseWebAppService<LegoFigureMakerProtocol>()
      , _legoRepository(std::move(legoRepository)) {
  }

  bool canHandle(std::shared_ptr<RequestType> request) override {
    LegoMessageCode code = request->messageCode;
    std::string figure = request->figure;

    std::regex findImage("Lego/");
    std::smatch match;
    bool isImage = std::regex_search(figure.cbegin(), figure.cend(), match, findImage);

    return code == LegoMessageCode::LEGO_REQUEST && !isImage;
  }

  std::optional<HarborCLS::MiddlewareMessage<std::shared_ptr<ResponseType>>> handleTask(
      std::shared_ptr<RequestType> request) override {
    std::optional<std::reference_wrapper<LegoFigure>> figure = _legoRepository->getFigureByName(request->figure);

    if (!figure.has_value()) {
      return std::nullopt;
    }

    HarborCLS::HtmlElement table("table");
    table.addChild(getTableHeader());

    for (const auto& piece : figure->get().legoPieces) {
      table.addChild(getPieceRow(piece));
    }

    std::shared_ptr<ResponseType> response = std::make_shared<ResponseType> (
      request->socket
      , LegoMessageCode::LEGO_RESPONSE
      , table.toString()
    );

    return HarborCLS::MiddlewareMessage<std::shared_ptr<ResponseType>>(response);
  }

  static std::shared_ptr<HarborCLS::HtmlElement> getPieceRow(const LegoPiece& piece) {
    std::shared_ptr<HarborCLS::HtmlElement> tableRow = std::make_shared<HarborCLS::HtmlElement>("tr");

    std::shared_ptr<HarborCLS::HtmlElement> quantity = std::make_shared<HarborCLS::HtmlElement>("td");
    quantity->addContent(std::to_string(piece.quantity));
    tableRow->addChild(quantity);

    std::shared_ptr<HarborCLS::HtmlElement> description = std::make_shared<HarborCLS::HtmlElement>("td");
    description->addContent(piece.pieceName);
    tableRow->addChild(description);

    std::shared_ptr<HarborCLS::HtmlElement> image = std::make_shared<HarborCLS::HtmlElement>("td");
    tableRow->addChild(image);

    std::shared_ptr<HarborCLS::HtmlElement> imageElement = std::make_shared<HarborCLS::HtmlElement>("img");
    imageElement->addProperty("src", piece.imagePath);
    imageElement->addProperty("alt", piece.pieceName);
    image->addChild(imageElement);

    return tableRow;
  }

  static std::shared_ptr<HarborCLS::HtmlElement> getTableHeader() {
    std::shared_ptr<HarborCLS::HtmlElement> headerRow = std::make_shared<HarborCLS::HtmlElement>("tr");

    std::shared_ptr<HarborCLS::HtmlElement> quantityHeader = std::make_shared<HarborCLS::HtmlElement>("th");
    quantityHeader->addContent("Cantidad");
    headerRow->addChild(quantityHeader);

    std::shared_ptr<HarborCLS::HtmlElement> descriptionHeader = std::make_shared<HarborCLS::HtmlElement>("th");
    descriptionHeader->addContent("Descripcion");
    headerRow->addChild(descriptionHeader);

    std::shared_ptr<HarborCLS::HtmlElement> pictureHeader = std::make_shared<HarborCLS::HtmlElement>("th");
    pictureHeader->addContent("Imagen");
    headerRow->addChild(pictureHeader);

    return headerRow;
  }
};


#endif //HARBOR_CLS_FIGURERETRIEVALSERVICE_HPP
