//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_IWEBAPPHANDLERREGISTRATION_HPP
#define HARBOR_CLS_IWEBAPPHANDLERREGISTRATION_HPP

#include <memory>
#include <optional>

namespace HarborCLS {
  template<typename RequestType, typename ResponseType>
  class IWebAppHandlerRegistration {
  public:
    virtual ~IWebAppHandlerRegistration() = default;

    virtual std::optional<std::shared_ptr<ResponseType>> executeHandler(std::shared_ptr<RequestType> request) = 0;
  };

}

#endif //HARBOR_CLS_IWEBAPPHANDLERREGISTRATION_HPP
