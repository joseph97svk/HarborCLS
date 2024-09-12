//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LEGOFIGUREMAKERPROTOCOL_HPP
#define HARBOR_CLS_LEGOFIGUREMAKERPROTOCOL_HPP

#include "../../../HarborCLS_Library/HarborCLS.hpp"
#include <HarborCLS.hpp>

#include "LFMRequest.hpp"
#include "LFMResponse.hpp"
#include "LFMRequestParser.hpp"
#include "LFMResponseComposer.hpp"

#include "LFMProtocolHeader.hpp"

struct LegoFigureMakerProtocol {
  using SocketType = HarborCLS::TcpSocket;
  using RequestType = LFMRequest;
  using ResponseType = LFMResponse;
  using RequestParserType = LFMRequestParser;
  using ResponseComposerType = LFMResponseComposer;
};

#endif //HARBOR_CLS_LEGOFIGUREMAKERPROTOCOL_HPP
