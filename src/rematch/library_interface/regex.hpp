#ifndef LIBRARY_INTERFACE__REGEX_HPP
#define LIBRARY_INTERFACE__REGEX_HPP

#include <optional>
#include <string>
#include <vector>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "mediator/mediation_subjects.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "evaluation/start_end_chars.hpp"
#include "statistics.hpp"

namespace REMatch {
inline namespace library_interface {

class Regex {
 private:
  Flags flags_;
  rematch::MediationSubjects mediation_subjects_;
  rematch::SegmentManagerCreator segment_manager_creator_;

 public:
  Regex(std::string_view pattern, Flags flags = Flags());

  std::unique_ptr<Match> find(std::string_view text, Flags flags = Flags());
  MatchIterator finditer(std::string_view text, Flags flags = Flags());
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
