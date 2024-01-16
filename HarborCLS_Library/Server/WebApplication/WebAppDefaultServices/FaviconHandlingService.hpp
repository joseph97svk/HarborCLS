//
// Created by josephvalverde on 1/16/24.
//

#ifndef HARBOR_CLS_FAVICONHANDLINGSERVICE_HPP
#define HARBOR_CLS_FAVICONHANDLINGSERVICE_HPP

#include <utility>
#include <fstream>

#include "Server/Http/HttpProtocol.hpp"
#include "Server/WebApplication/DependencyManagement/BaseWebAppService.hpp"
#include "IConfiguration.hpp"
#include "Server/ServerConfiguration.hpp"

namespace HarborCLS {

  class FaviconHandlingService : public BaseWebAppService<HttpProtocol> {
    std::shared_ptr<IConfiguration<ServerConfiguration>> _configurationService;

  public:
    explicit FaviconHandlingService(std::shared_ptr<IConfiguration<ServerConfiguration>> configuration)
        : BaseWebAppService<HttpProtocol>()
        , _configurationService(std::move(configuration)) {}

    [[nodiscard]] bool canHandle(std::shared_ptr<HttpProtocol::RequestType> request) override {
      HttpMappings::Method method = request->header.method;
      std::string_view path = request->header.url;

      return method == HttpMappings::Method::GET
          && path == "/favicon.ico";
    }

    [[nodiscard]] std::optional<MiddlewareMessage<std::shared_ptr<HttpProtocol::ResponseType>>> handleTask(
        std::shared_ptr<HttpProtocol::RequestType> request) override {
      auto response = std::make_shared<HttpProtocol::ResponseType>();

      std::optional<std::string> faviconFilePath = _configurationService->get("faviconPath");

      if (!faviconFilePath) {
        return std::nullopt;
      }

      std::ifstream faviconFile(*faviconFilePath, std::ios::binary);

      if (!faviconFile.is_open()) {
        return std::nullopt;
      }

      std::vector<char> faviconData((std::istreambuf_iterator<char>(faviconFile)),
                                    std::istreambuf_iterator<char>());

      response->socket = request->socket;
      response->contentLength = faviconData.size();

      response->body = std::move(faviconData);
      response->contentType = ContentType::ImagePng;
      response->otherHeaderFields.emplace_back(AdditionalField{"Content-Transfer-Encoding", "binary"});

      faviconFile.close();

      return MiddlewareMessage<std::shared_ptr<HttpProtocol::ResponseType>>(response);
    }

  };

}

#endif //HARBOR_CLS_FAVICONHANDLINGSERVICE_HPP
