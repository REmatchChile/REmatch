#ifndef LIBRARY_INTERFACE__REGEX_HPP
#define LIBRARY_INTERFACE__REGEX_HPP

#include <optional>
#include <string>
#include <vector>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "mediator/mediation_subjects.hpp"

namespace REMatch {
inline namespace library_interface {

class Regex {
 private:
  rematch::MediationSubjects mediation_subjects_;

 public:
  Regex(std::string_view pattern, Flags flags = Flags());
  static void set_memory_limit(size_t bytes);
  static size_t get_memory_limit();

  std::unique_ptr<Match> find(std::string_view text, Flags flags = Flags());
  MatchIterator finditer(std::string_view text, Flags flags = Flags());
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
