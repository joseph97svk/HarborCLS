//
// Created by josephvalverde on 12/11/23.
//

#include "IHttpRequestParser.hpp"
#include "../Socket/TcpSocket.hpp"

class HttpRequestParser : public IHttpRequestParser<TcpSocket> {
public:
  [[nodiscard]] std::shared_ptr<HttpRequest> createHttpRequest(std::vector<char>& request, std::shared_ptr<TcpSocket>) override;
};