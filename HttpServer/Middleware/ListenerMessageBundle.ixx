//
// Created by josephvalverde on 12/11/23.
//

export module ListenerMessageBundle;
import <string>;

export struct ListenerMessageBundle {
    std::string listeningMessage;
    std::string afterReceivedMessage;
    std::string stopMessage;
};