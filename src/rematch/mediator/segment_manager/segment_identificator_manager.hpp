#ifndef SEGMENT_IDENTIFICATOR_MANAGER_HPP
#define SEGMENT_IDENTIFICATOR_MANAGER_HPP

#include "segment_manager.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

namespace REmatch {

class SegmentIdentificatorManager : public SegmentManager {
 public:
  SegmentIdentificatorManager(SearchDFA& search_dfa, std::shared_ptr<Document> document);

  std::unique_ptr<Span> next() override;
  size_t get_search_dfa_size() override;
  size_t get_search_nfa_size() override;

 private:
  SegmentIdentificator segment_identificator_;
};

}

#endif
