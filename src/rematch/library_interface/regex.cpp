#include "regex.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/output_checker.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(const std::string& pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::findone(const std::string& text) {
  std::string document = rematch::add_start_and_end_chars(text);

  std::unique_ptr<rematch::FindoneMediator> mediator =
      std::make_unique<rematch::FindoneMediator>(regex_data_, document, flags_);

  rematch::mediator::Mapping* mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, regex_data_.variable_catalog,
                                   document);
  }

  return nullptr;
}

std::unique_ptr<MatchIterator> Regex::finditer(
    const std::string& document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  return std::make_unique<MatchIterator>(regex_data_, std::move(document),
                                         flags_);
}

bool Regex::check(const std::string& document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  std::unique_ptr<rematch::OutputChecker> mediator =
      std::make_unique<rematch::OutputChecker>(regex_data_, document, flags_);

  return mediator->check();
}

}  // end namespace library_interface
}  // namespace REMatch
