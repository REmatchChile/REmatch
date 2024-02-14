#ifndef FINDITER_MEDIATOR_HPP
#define FINDITER_MEDIATOR_HPP

#include <string_view>
#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"

namespace rematch {

class CountMediator : public Mediator {
 public:
  CountMediator(RegexData& regex_data, std::shared_ptr<Document> document,
                Flags flags = Flags())
      : Mediator(regex_data, document){
            // TODO: Implementar esta clase.
        };

  mediator::Mapping* next() override{};
};

}  // namespace rematch

#endif
