//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_PIECESSERVER_HPP
#define HARBOR_CLS_PIECESSERVER_HPP

#include <HarborCLS.hpp>
#include "../../HarborCLS_Library/HarborCLS.hpp"

#include "../LegoFigureMakerCommon/Protocol/LegoFigureMakerProtocol.hpp"

/*class PiecesServer : public HarborCLS::GenericServer<LegoFigureMakerProtocol> {
};*/

using PiecesServer = HarborCLS::GenericServer<LegoFigureMakerProtocol>;

using PiecesWebApplication = HarborCLS::GenericWebApplication<LegoFigureMakerProtocol>;

#endif //HARBOR_CLS_PIECESSERVER_HPP
