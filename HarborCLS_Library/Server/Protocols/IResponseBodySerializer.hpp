//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_IRESPONSEBODYSERIALIZER_HPP
#define HARBOR_CLS_IRESPONSEBODYSERIALIZER_HPP

#include <vector>

namespace HarborCLS {

template<typename ResponseType>
class IResponseBodySerializer {
public:
  virtual ~IResponseBodySerializer() = default;

  [[nodiscard]] virtual std::vector<char> serialize(ResponseType& response) = 0;
};

}

#endif //HARBOR_CLS_IRESPONSEBODYSERIALIZER_HPP
