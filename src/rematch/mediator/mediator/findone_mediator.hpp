#ifndef FINDONE_MEDIATOR_HPP
#define FINDONE_MEDIATOR_HPP

#include <string_view>
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"

namespace rematch {

class FindoneMediator : public Mediator {
 public:
  FindoneMediator(RegexData& regex_data, std::string_view document,
                  Flags flags = Flags());

  mediator::Mapping* next() override;

 private:
  bool has_output = false;
};

}  // namespace rematch

#endif
