#include "regex.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/output_checker.hpp"

#include "evaluation/document.hpp"

using namespace rematch;

namespace REMatch {
inline namespace library_interface {

Regex::Regex(std::string_view pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::findone(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  std::unique_ptr<rematch::FindoneMediator> mediator =
      std::make_unique<rematch::FindoneMediator>(regex_data_, document, flags_);

  rematch::mediator::Mapping* mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, regex_data_.variable_catalog,
                                   document);
  }

  return nullptr;
}

std::unique_ptr<MatchIterator> Regex::finditer(std::string_view text) {
  return std::make_unique<MatchIterator>(regex_data_, text, flags_);
}

bool Regex::check(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto output_checker = rematch::OutputChecker(regex_data_, document);
  return output_checker.check();
}

size_t Regex::count(std::string_view) {
  return 0;
}

}  // end namespace library_interface
}  // namespace REMatch
