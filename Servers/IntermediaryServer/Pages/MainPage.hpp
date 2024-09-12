//
// Created by josephvalverde on 1/3/24.
//

#ifndef HARBOR_CLS_MAINPAGE_HPP
#define HARBOR_CLS_MAINPAGE_HPP

#include "../../../HarborCLS_Library/Server/Http/WebApplication/Services/MVC/HttpController.hpp"
#include "../Services/FiguresService.hpp"

class MainPage : public HarborCLS::HttpController {
  std::shared_ptr<FiguresService> _figuresService;

public:
  explicit MainPage(std::shared_ptr<FiguresService> figuresService)
      : HttpController(
          "Servers/IntermediaryServer/Pages/MainPage.html"
          , "Servers/IntermediaryServer/Pages/layout.html")
          , _figuresService(std::move(figuresService)){

    this->registerGetHandler("", &MainPage::getFigures, *this);
  }

  std::optional<std::shared_ptr<HarborCLS::HttpResponse>> getFigures(std::shared_ptr<HarborCLS::HttpRequest> request) {
    std::vector<std::string> figures = _figuresService->getFigures();

    HarborCLS::HtmlElement figuresForm("form");

    std::shared_ptr<HarborCLS::HtmlElement> select = std::make_shared<HarborCLS::HtmlElement>("select");
    select->addProperty("onChange",  "redirectToFigure(this)");
    figuresForm.addChild(select);

    std::shared_ptr<HarborCLS::HtmlElement> chooseOption = std::make_shared<HarborCLS::HtmlElement>("option");
    chooseOption->addContent("Elegir");
    select->addChild(chooseOption);

    for (auto& figure : figures) {
      std::shared_ptr<HarborCLS::HtmlElement> option = std::make_shared<HarborCLS::HtmlElement>("option");
      option->addProperty("value", figure);
      option->addContent(figure);

      select->addChild(option);
    }

    auto response = std::make_shared<HarborCLS::HttpResponse>();

    response->socket = request->socket;

    std::string form = figuresForm.toString();

    this->_view->replace("SelectFiguresForm", form);

    auto contents = this->_view->getHtmlContents();
    if (!contents) {
      return std::nullopt;
    }
    response->body = *contents;

    return response;
  }


};

#endif //HARBOR_CLS_MAINPAGE_HPP
