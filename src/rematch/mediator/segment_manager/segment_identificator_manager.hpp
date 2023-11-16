#ifndef SEGMENT_IDENTIFICATOR_MANAGER_HPP
#define SEGMENT_IDENTIFICATOR_MANAGER_HPP

#include "segment_manager.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

namespace rematch {

class SegmentIdentificatorManager : public SegmentManager {
 public:
  SegmentIdentificatorManager(SearchDFA& search_dfa, std::string_view document);

  std::unique_ptr<Span> next() override;

 private:
  SegmentIdentificator segment_identificator_;
};

}

#endif
