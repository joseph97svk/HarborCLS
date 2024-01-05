//
// Created by josephvalverde on 12/11/23.
//

#include "Server/Protocols/IRequestParser.hpp"
#include "../../Socket/TcpSocket.hpp"

namespace HarborCLS {

  class HttpRequestParser : public IRequestParser<TcpSocket, HttpRequest> {
  public:
    [[nodiscard]] std::shared_ptr<HttpRequest>
    createHttpRequest(std::vector<char> &request, std::shared_ptr<TcpSocket>) override;
  };
}