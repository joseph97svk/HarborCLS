//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LEGODISCOVERHANDLER_HPP
#define HARBOR_CLS_LEGODISCOVERHANDLER_HPP

#include <HarborCLS.hpp>

#include "../Services/LegoInMemoryRepository.hpp"
#include "../../LegoFigureMakerCommon/Protocol/LegoFigureMakerProtocol.hpp"

class LegoDiscoverHandler : public HarborCLS::Handler<std::shared_ptr<std::vector<char>>> {
  std::shared_ptr<ILegoRepository> _legoInMemoryRepository;

public:
  explicit LegoDiscoverHandler(
      HarborCLS::MiddlewareBlockingQueue<std::shared_ptr<std::vector<char>>> &consumingQueue
      , std::shared_ptr<HarborCLS::ILogger> logger
      , std::shared_ptr<ILegoRepository> legoInMemoryRepository)
      : HarborCLS::Handler<std::shared_ptr<std::vector<char>>>(consumingQueue, std::move(logger))
      , _legoInMemoryRepository(std::move(legoInMemoryRepository)) {
  }

  void optionalToEnd() override {
  }

  void handleSingle(std::shared_ptr<std::vector<char>> data) override {
    auto code = static_cast<LegoMessageCode>((*data)[0]);
    (void) code;

    std::string message(data->begin() + 2, data->end());

    std::string ip = message.substr(0, message.find(':'));
    std::string port = message.substr(message.find(':') + 1, message.size());

    std::vector<char> response { std::to_string(LEGO_PRESENT)[0], SEPARATOR };

    std::string ipToSend = HarborCLS::getComputerIp();
    std::string portToSend = std::to_string(PIECES_TCP_PORT);

    response.insert(response.end(), ipToSend.begin(), ipToSend.end());
    response.push_back(':');
    response.insert(response.end(), portToSend.begin(), portToSend.end());
    response.push_back(SEPARATOR);

    std::vector<std::string> legoList = _legoInMemoryRepository->getAllNames();

    for (const std::string& legoName : legoList) {
      response.insert(response.end(), legoName.begin(), legoName.end());
      response.push_back(SEPARATOR);
    }

    HarborCLS::UDPSocket socket;

    socket[ip, stoi(port)] << response;
  }

};
#endif //HARBOR_CLS_LEGODISCOVERHANDLER_HPP
