#include "multi_regex.hpp"
#include "evaluation/document.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/multi_match_iterator.hpp"
#include "library_interface/regex_data/regex_data.hpp"
#include "library_interface/regex_data/regex_data_utils.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "mediator/output_checker.hpp"

namespace REMatch {

MultiRegex::MultiRegex(std::string_view pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<MultiMatch> MultiRegex::findone(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto mediator = MultiFindoneMediator(regex_data_, document, flags_);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();

  if (mapping != nullptr) {
    return std::make_unique<MultiMatch>(*mapping, regex_data_.variable_catalog,
                                        document);
  }
  return nullptr;
}

std::unique_ptr<REMatch::MultiMatchIterator> MultiRegex::finditer(
    std::string_view text) {
  return std::make_unique<REMatch::MultiMatchIterator>(regex_data_, text,
                                                       flags_);
}

bool MultiRegex::check(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);
  auto output_checker = OutputChecker(regex_data_, document);
  return output_checker.check();
}

}  // namespace REMatch
