//
// Created by josephvalverde on 12/19/23.
//

#ifndef HARBOR_CLS_PROTOCOLCONCEPT_HPP
#define HARBOR_CLS_PROTOCOLCONCEPT_HPP

template<typename ProtocolType>
concept ServerProtocol = requires {
  typename ProtocolType::SocketType;
  typename ProtocolType::RequestType;
  typename ProtocolType::ResponseType;
  typename ProtocolType::RequestParserInterface;
  typename ProtocolType::RequestParserType;
  typename ProtocolType::ResponseHeaderComposerInterface;
  typename ProtocolType::ResponseHeaderComposerType;
};

#endif //HARBOR_CLS_PROTOCOLCONCEPT_HPP
