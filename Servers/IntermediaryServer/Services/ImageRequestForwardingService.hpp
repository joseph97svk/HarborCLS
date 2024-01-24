//
// Created by josephvalverde on 1/17/24.
//

#ifndef HARBOR_CLS_IMAGEREQUESTFORWARDINGSERVICE_HPP
#define HARBOR_CLS_IMAGEREQUESTFORWARDINGSERVICE_HPP

#include <HarborCLS.hpp>

#include "FiguresService.hpp"

class ImageRequestForwardingService : public HarborCLS::BaseWebAppService<HarborCLS::HttpProtocol> {
  static constexpr std::string supportedImageExtensions[] =
      {".png", ".jpg", ".jpeg", ".gif", ".bmp"};

  std::shared_ptr<FiguresService> _figuresService;

public:
  explicit ImageRequestForwardingService(std::shared_ptr<FiguresService> figuresService)
      : HarborCLS::BaseWebAppService<HarborCLS::HttpProtocol>()
      , _figuresService(std::move(figuresService)) {
  }

  bool canHandle(std::shared_ptr<HarborCLS::HttpRequest> request) override {
    HarborCLS::HttpHeader& header = request->header;
    std::string url = header.url;

    bool isPicture = false;
    if (header.fields.contains(HarborCLS::HttpMappings::HeaderField::expectedResponseType)) {
      isPicture = header.fields.at(HarborCLS::HttpMappings::HeaderField::expectedResponseType).contains("image");
    }
    if (header.fields.contains(HarborCLS::HttpMappings::HeaderField::contentType)) {
      isPicture =
          isPicture
          && header.fields.at(HarborCLS::HttpMappings::HeaderField::contentType).contains("image");
    }

    bool isFavicon = url.contains("favicon.ico");

    bool extensionIsSupported = false;
    for (const std::string& extension : supportedImageExtensions) {
      if (url.contains(extension)) {
        extensionIsSupported = true;
        break;
      }
    }

    return isPicture && !isFavicon && extensionIsSupported;
  }

  std::optional<HarborCLS::MiddlewareMessage<std::shared_ptr<HarborCLS::HttpResponse>>> handleTask(
      std::shared_ptr<HarborCLS::HttpRequest> request) override {
    std::string url = request->header.url;

    std::optional<std::vector<char>> image = _figuresService->getLegoImage(url);
    std::shared_ptr<HarborCLS::HttpResponse> response = std::make_shared<HarborCLS::HttpResponse>();

    if (!image) {
      response->body = "404: Image not found";
      response->statusCode = HarborCLS::StatusCode::NotFound;
      response->socket = request->socket;
      return HarborCLS::MiddlewareMessage<std::shared_ptr<HarborCLS::HttpResponse>>(response);
    }

    response->contentType = HarborCLS::ContentType::ImagePng;
    response->contentLength = image.value().size();
    response->body = std::move(image.value());
    response->socket = request->socket;

    return HarborCLS::MiddlewareMessage<std::shared_ptr<HarborCLS::HttpResponse>>(response);
  }
};


#endif //HARBOR_CLS_IMAGEREQUESTFORWARDINGSERVICE_HPP
