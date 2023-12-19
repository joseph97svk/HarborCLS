//
// Created by josephvalverde on 12/18/23.
//

#ifndef LEGO_FIGURE_MAKER_IRESPONSEHEADERCOMPOSER_HPP
#define LEGO_FIGURE_MAKER_IRESPONSEHEADERCOMPOSER_HPP

#include <string>

#include "Http/HttpMessages/Reponse/HttpResponse.hpp"

class IResponseHeaderComposer {
public:
    virtual ~IResponseHeaderComposer() = default;

    virtual std::string composeHeader(std::shared_ptr<HttpResponse>) = 0;
};

#endif //LEGO_FIGURE_MAKER_IRESPONSEHEADERCOMPOSER_HPP
