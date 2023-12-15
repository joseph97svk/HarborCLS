//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_NOLOGRATATION_HPP
#define LEGO_FIGURE_MAKER_NOLOGRATATION_HPP

#include "ILogFileRotation.hpp"

class NoLogFileRotation : public ILogFileRotation {
public:
    NoLogFileRotation() = default;

    ~NoLogFileRotation() override = default;

    void rotateLogFile(std::ofstream& file, std::string& fileName) override {
      (void) file;
      (void) fileName;
    }
};

#endif //LEGO_FIGURE_MAKER_NOLOGRATATION_HPP
