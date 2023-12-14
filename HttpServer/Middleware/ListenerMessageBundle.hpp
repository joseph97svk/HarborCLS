//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_LISTENERMESSAGEBUNDLE_HPP
#define HTTPSERVER_LISTENERMESSAGEBUNDLE_HPP

#include <string>

struct ListenerMessageBundle {
    std::string listeningMessage;
    std::string afterReceivedMessage;
    std::string stopMessage;
};

#endif //HTTPSERVER_LISTENERMESSAGEBUNDLE_HPP