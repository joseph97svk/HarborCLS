//
// Created by josephvalverde on 12/18/23.
//

#ifndef LEGO_FIGURE_MAKER_HTTPBEHAVIOR_HPP
#define LEGO_FIGURE_MAKER_HTTPBEHAVIOR_HPP

#include "Http/HttpRequestParser/IHttpRequestParser.hpp"
#include "Http/ResponseHeaderComposer/IResponseHeaderComposer.hpp"


struct HttpBehavior {
  std::shared_ptr<IHttpRequestParser<TcpSocket>> requestParser;
  std::shared_ptr<IResponseHeaderComposer> responseHeaderComposer;
};

#endif //LEGO_FIGURE_MAKER_HTTPBEHAVIOR_HPP
