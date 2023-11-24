#ifndef LINE_BY_LINE_MANAGER_HPP
#define LINE_BY_LINE_MANAGER_HPP

#include <memory>
#include "line_identificator.hpp"
#include "segment_manager.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace rematch {

class LineByLineManager : public SegmentManager {
 public:
  LineByLineManager(std::unique_ptr<SearchDFA> search_dfa, std::string_view document);

  std::unique_ptr<Span> next() override;

 private:
  void update_line_in_segment_identificator(Span& line_span);

  SegmentIdentificator segment_identificator_;
  LineIdentificator line_identificator_;
  std::string document_;
  int shift_ = 0;
};

}  // namespace rematch

#endif
