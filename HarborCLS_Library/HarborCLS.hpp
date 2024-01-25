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
#include "Server/Protocols/IResponseComposer.hpp"

#include "Server/Socket/TcpSocket.hpp"
#include "Server/Socket/UdpSocket.hpp"
#include "Server/Middleware/Handler.hpp"
#include "Server/Middleware/Listener.hpp"
#include "Server/Middleware/MiddlewareMessage.hpp"

#include "Server/WebApplication/DependencyManagement/BuilderReferenceWrapper.hpp"
#include "Server/WebApplication/WebAppDefaultServices/MVC/Controller/BaseController.hpp"
#include "Server/WebApplication/WebAppDefaultServices/MVC/View/BaseView.hpp"
#include "Server/WebApplication/WebAppDefaultServices/MVC/View/HtmlElement.hpp"
#include "Server/WebApplication/WebAppDefaultServices/DefaultResourcesProviderService.hpp"
#include "Server/WebApplication/WebAppDefaultServices/StaticImageProviderService.hpp"

// for all logger related includes
#include "Logger/LoggerFactory/LoggerFactory.hpp"

// for all json reader includes
#include "JsonReader/JsonHandler.hpp"
#include "JsonReader/ParsingPolicy.hpp"

// common
#include "Common/common.hpp"

#endif //HARBOR_CLS_HARBORCLS_HPP
