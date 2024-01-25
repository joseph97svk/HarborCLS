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
    table.addProperty("border", "1");
    table.addProperty("cellpadding", "5");
    table.addProperty("align", "center");

    auto figurePictureRow = HarborCLS::HtmlElement::create("tr");
    table.addChild(figurePictureRow);

    auto figurePictureCol = HarborCLS::HtmlElement::create("td");
    figurePictureCol->addProperty("colspan", "3");
    figurePictureRow->addChild(figurePictureCol);

    auto figurePicture = HarborCLS::HtmlElement::create("img");
    figurePictureCol->addChild(figurePicture);
    figurePicture->addProperty("src", figure->get().imagePath);
    figurePicture->addProperty("width", "500");
    figurePicture->addProperty("height", "500");

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
    auto tableRow = HarborCLS::HtmlElement::create("tr");

    auto quantity = HarborCLS::HtmlElement::create("td");
    quantity->addContent(std::to_string(piece.quantity));
    tableRow->addChild(quantity);

    auto description = HarborCLS::HtmlElement::create("td");
    description->addContent(piece.pieceName);
    tableRow->addChild(description);

    auto image = HarborCLS::HtmlElement::create("td");
    tableRow->addChild(image);

    auto imageElement = HarborCLS::HtmlElement::create("img");
    imageElement->addProperty("src", piece.imagePath);
    imageElement->addProperty("width", "100");
    imageElement->addProperty("height", "100");
    imageElement->addProperty("alt", piece.pieceName);
    image->addChild(imageElement);

    return tableRow;
  }

  static std::shared_ptr<HarborCLS::HtmlElement> getTableHeader() {
    auto headerRow = HarborCLS::HtmlElement::create("tr");

    auto quantityHeader = HarborCLS::HtmlElement::create("th");
    quantityHeader->addContent("Cantidad");
    headerRow->addChild(quantityHeader);

    auto descriptionHeader = HarborCLS::HtmlElement::create("th");
    descriptionHeader->addContent("Descripcion");
    headerRow->addChild(descriptionHeader);

    auto pictureHeader = HarborCLS::HtmlElement::create("th");
    pictureHeader->addContent("Imagen");
    headerRow->addChild(pictureHeader);

    return headerRow;
  }
};


#endif //HARBOR_CLS_FIGURERETRIEVALSERVICE_HPP
