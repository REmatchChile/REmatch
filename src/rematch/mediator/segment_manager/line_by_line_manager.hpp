#ifndef LINE_BY_LINE_MANAGER_HPP
#define LINE_BY_LINE_MANAGER_HPP

#include <memory>
#include "line_identificator.hpp"
#include "segment_manager.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace REmatch {
class Document;

class LineByLineManager : public SegmentManager {
 public:
  LineByLineManager(SearchDFA& search_dfa, std::shared_ptr<Document> document);

  std::unique_ptr<Span> next() override;
  size_t get_search_dfa_size() override;
  size_t get_search_nfa_size() override;

 private:
  void update_line_in_segment_identificator(Span& line_span);

  SegmentIdentificator segment_identificator_;
  LineIdentificator line_identificator_;
  std::shared_ptr<Document> document_;
};

}  // namespace REmatch

#endif
