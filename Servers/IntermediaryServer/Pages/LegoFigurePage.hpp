//
// Created by josephvalverde on 1/16/24.
//

#ifndef HARBOR_CLS_LEGOFIGUREPAGE_HPP
#define HARBOR_CLS_LEGOFIGUREPAGE_HPP

#include <HarborCLS.hpp>
#include "../Services/FiguresService.hpp"

class LegoFigurePage : public HarborCLS::HttpController {
  std::shared_ptr<FiguresService> _figuresService;

public:
  explicit LegoFigurePage(std::shared_ptr<FiguresService> figuresService)
      : HttpController(
          "Servers/IntermediaryServer/Pages/LegoFigurePage.html"
          , "Servers/IntermediaryServer/Pages/layout.html")
          , _figuresService(std::move(figuresService)){

    this->registerGetHandler("figure", &LegoFigurePage::serveFigure, *this);
  }

  std::optional<std::shared_ptr<HarborCLS::HttpResponse>> serveFigure(std::shared_ptr<HarborCLS::HttpRequest> request) {
    std::string_view figureNameBuffer = std::string_view(request->header.url).substr(1);
    figureNameBuffer.remove_prefix(figureNameBuffer.find('=') + 1);
    std::string figureName = std::string(figureNameBuffer);

    std::optional<std::string> figureTable = _figuresService->getFigure(figureName);

    if (!figureTable) {
      return HttpController::NotFound404Response(request);
    }

    this->_view->replace("FigureName", figureName);
    this->_view->replace("FigureTable", *figureTable);

    std::shared_ptr<HarborCLS::HttpResponse> response = std::make_shared<HarborCLS::HttpResponse>();
    auto contents = this->_view->getHtmlContents();

    if (!contents) {
      return std::nullopt;
    }

    response->body = *contents;
    response->socket = request->socket;

    return response;
  }
};

#endif //HARBOR_CLS_LEGOFIGUREPAGE_HPP
