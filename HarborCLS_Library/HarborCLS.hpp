//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_HARBORCLS_HPP
#define HARBOR_CLS_HARBORCLS_HPP

// for all server related includes
#include "Server/GenericServer.hpp"
#include "Server/WebApplication/GenericWebApplication.hpp"

#include "Server/Http/HttpServer.hpp"
#include "Server/Http/WebApplication/HttpWebApplication.hpp"

#include "Server/Protocols/ProtocolConcept.hpp"
#include "Server/Protocols/IRequestParser.hpp"
#include "Server/Protocols/IResponseHeaderComposer.hpp"

#include "Server/Socket/TcpSocket.hpp"
#include "Server/Middleware/Handler.hpp"
#include "Server/Middleware/Listener.hpp"

// for all logger related includes
#include "Logger/LoggerFactory/LoggerFactory.hpp"

// for all json reader includes
#include "JsonReader/JsonHandler.hpp"
#include "JsonReader/ParsingPolicy.hpp"

#endif //HARBOR_CLS_HARBORCLS_HPP
