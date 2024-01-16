//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_DEFAULTRESOURCESPROVIDERSERVICE_HPP
#define HARBOR_CLS_DEFAULTRESOURCESPROVIDERSERVICE_HPP

#include <fstream>

#include "../DependencyManagement/BaseWebAppService.hpp"
#include "Server/Http/HttpProtocol.hpp"

namespace HarborCLS {
  class DefaultResourcesProviderService : public BaseWebAppService<HttpProtocol> {

  public:
    DefaultResourcesProviderService()
        : BaseWebAppService<HttpProtocol>() {}

    [[nodiscard]] bool canHandle(std::shared_ptr<HttpProtocol::RequestType> request) override {
      HttpMappings::Method method = request->header.method;
      std::string_view path = request->header.url;

      return method == HttpMappings::Method::GET
        && path.ends_with(".js")
        || path.ends_with(".ts")
        || path.ends_with(".css") ;
    }

    [[nodiscard]] std::optional<MiddlewareMessage<std::shared_ptr<HttpProtocol::ResponseType>>> handleTask(
        std::shared_ptr<HttpProtocol::RequestType> request) override {
      auto response = std::make_shared<HttpProtocol::ResponseType>();
      response->socket = request->socket;

      std::string_view path = request->header.url;
      path.remove_prefix(1); // remove the first '/' from the path

      std::ifstream file((std::string(path)));

      if (!file.is_open()) {
        return std::nullopt;
      }

      std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

      content.push_back('\n');

      response->body = content;

      std::string_view extension = path.substr(path.find_last_of('.') + 1);

      if (extension == "js" || extension == "ts") {
        response->contentType = ContentType::ApplicationJavascript;
      } else if (extension == "css") {
        response->contentType = ContentType::TextCss;
      }

      file.close();

      return MiddlewareMessage<std::shared_ptr<HttpProtocol::ResponseType>>(response);
    }
  };
}
#endif //HARBOR_CLS_DEFAULTRESOURCESPROVIDERSERVICE_HPP
