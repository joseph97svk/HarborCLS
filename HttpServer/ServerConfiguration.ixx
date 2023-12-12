//
// Created by josephvalverde on 12/11/23.
//

export module ServerConfiguration;

import <string>;

export struct ServerConfiguration {
    unsigned int port;

    unsigned int requestHandlerAmount;
    unsigned int applicationHandlerAmount;
    unsigned int responseHandlerAmount;

    std::string sslCertFileName;
    std::string sslKeyFileName;

    bool isSslEnabled () {
        return !sslCertFileName.empty() && !sslKeyFileName.empty();
    }
};