//
// Created by josephvalverde on 1/3/24.
//

#ifndef HARBOR_CLS_MAINPAGE_HPP
#define HARBOR_CLS_MAINPAGE_HPP

#include "../../../HarborCLS_Library/Server/Http/WebApplication/Services/MVC/HttpController.hpp"


class MainPage : public HarborCLS::HttpController {
public:
  MainPage()
      : HttpController(
          "Servers/IntermediaryServer/Pages/MainPage.html"
          , "Servers/IntermediaryServer/Pages/layout.html") {
  }
};

#endif //HARBOR_CLS_MAINPAGE_HPP
