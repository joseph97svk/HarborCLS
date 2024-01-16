//
// Created by josephvalverde on 12/25/23.
//

#ifndef HARBOR_CLS_BASECONTROLLER_HPP
#define HARBOR_CLS_BASECONTROLLER_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>

#include "Server/WebApplication/WebAppDefaultServices/MVC/View/BaseView.hpp"
#include "WebAppHandlerRegistration.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class BaseController {
  protected:
    using RequestType = typename Protocol::RequestType;
    using ResponseType = typename Protocol::ResponseType;

    std::optional<BaseView> _view {};

  public:
    template<typename ControllerImplementation>
    using HandlerFunction = std::optional<std::shared_ptr<ResponseType>> (ControllerImplementation::*)(std::shared_ptr<RequestType>);

    enum class ControllerError {
      GENERIC_ERROR,
      INVALID_HANDLER,
      INVALID_METHOD
    };

    explicit BaseController(const std::string& htmlFileName, std::string layoutFileName) noexcept(false){

      if (htmlFileName.empty()) {
        throw std::runtime_error("BaseController: html file name cannot be empty");
      }

      std::string extension = std::filesystem::path(htmlFileName).extension().string();

      if (extension != ".html" && extension != ".htm") {
        throw std::runtime_error("BaseController: html file must have .html or .htm extension");
      }

      if (!std::filesystem::exists(htmlFileName)) {
        throw std::runtime_error("BaseController: html file does not exist");
      }

      std::optional<std::string> layoutFileNameOpt =
          layoutFileName.empty() ?
          std::nullopt :
          std::optional<std::string>(layoutFileName);

      _view = BaseView(htmlFileName, layoutFileNameOpt);
    }

    virtual ~BaseController() = default;

    virtual std::expected<std::optional<std::shared_ptr<ResponseType>>, Error<ControllerError>> processRequest(
        const std::shared_ptr<RequestType>& RequestType
        , const std::string& handler) = 0;

  };
}

#endif //HARBOR_CLS_BASECONTROLLER_HPP
