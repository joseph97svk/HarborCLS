//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_IREQUESTPARSER_HPP
#define HARBOR_CLS_IREQUESTPARSER_HPP

#include <memory>

#include "../HttpMessages/Request/HttpRequest.hpp"

namespace HarborCLS {

  template<typename SocketType, typename RequestType>
  class IRequestParser {
  public:
    virtual ~IRequestParser() = default;

    [[nodiscard]] virtual std::shared_ptr<RequestType> createHttpRequest(
        std::vector<char> &request,
        std::shared_ptr<SocketType>) = 0;
  };
}
#endif //HARBOR_CLS_IREQUESTPARSER_HPP
