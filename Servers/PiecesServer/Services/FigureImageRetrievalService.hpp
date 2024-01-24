//
// Created by josephvalverde on 1/17/24.
//

#ifndef HARBOR_CLS_FIGUREIMAGERETRIEVALSERVICE_HPP
#define HARBOR_CLS_FIGUREIMAGERETRIEVALSERVICE_HPP

#include <HarborCLS.hpp>
#include <utility>
#include "Protocol/LegoFigureMakerProtocol.hpp"

class FigureImageRetrievalService : public HarborCLS::BaseWebAppService<LegoFigureMakerProtocol> {
  using ResponseType = typename LegoFigureMakerProtocol::ResponseType;
  using RequestType = typename LegoFigureMakerProtocol::RequestType;

public:
  explicit FigureImageRetrievalService()
      : HarborCLS::BaseWebAppService<LegoFigureMakerProtocol>() {}

  bool canHandle(std::shared_ptr<RequestType> request) override {
    LegoMessageCode code = request->messageCode;
    std::string figure = request->figure;

    std::regex findImage("/Lego/");
    std::smatch match;
    bool isImage = std::regex_search(figure.cbegin(), figure.cend(), match, findImage);

    return code == LegoMessageCode::LEGO_REQUEST && isImage;
  }

  std::optional<HarborCLS::MiddlewareMessage<std::shared_ptr<ResponseType>>> handleTask(
      std::shared_ptr<RequestType> request) override {
    std::string& url = request->figure;

    std::string_view figureLocation = url;
    figureLocation.remove_prefix(1);
    figureLocation.remove_prefix(figureLocation.find_first_of('/'));

    std::string path = "Servers/PiecesServer" + std::string(figureLocation);

    std::cout << "Path: " << path << std::endl;

    std::optional<std::vector<char>> image = getImage(path);

    if (!image.has_value()) {
      return std::nullopt;
    }

    std::shared_ptr<ResponseType> response = std::make_shared<ResponseType> (
      request->socket
      , LegoMessageCode::LEGO_RESPONSE
      , *image
    );

    return HarborCLS::MiddlewareMessage<std::shared_ptr<ResponseType>>(response);
  }

protected:
  static std::optional<std::vector<char>> getImage(std::string& path ) {
    if (path.empty()) {
      return std::nullopt;
    }

    std::fstream imageFile(path, std::ios::in | std::ios::binary);

    if (!imageFile.is_open()) {
      return std::nullopt;
    }

    std::vector<char> image ((std::istreambuf_iterator<char>(imageFile)),
                             (std::istreambuf_iterator<char>()));

    return image;
  }
};


#endif //HARBOR_CLS_FIGUREIMAGERETRIEVALSERVICE_HPP
