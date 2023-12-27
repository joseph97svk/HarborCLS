//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_HTTPWEBAPPLICATION_HPP
#define HARBOR_CLS_HTTPWEBAPPLICATION_HPP

#include "../../WebApplication/GenericWebApplication.hpp"
#include "../HttpProtocol.hpp"

namespace HarborCLS {

  class HttpWebApplication : public GenericWebApplication<HttpProtocol> {

  };

}

#endif //HARBOR_CLS_HTTPWEBAPPLICATION_HPP
