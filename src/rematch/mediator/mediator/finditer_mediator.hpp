#ifndef FINDITER_MEDIATOR_HPP
#define FINDITER_MEDIATOR_HPP

#include <string_view>
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"

namespace rematch {

class FinditerMediator : public Mediator {
 public:
  FinditerMediator(RegexData& regex_data, std::string_view document,
                   Flags flags = Flags());

  mediator::Mapping* next() override;

 private:
  bool next_is_computed_successfully();
};

}  // namespace rematch

#endif
