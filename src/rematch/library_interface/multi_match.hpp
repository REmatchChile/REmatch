#ifndef MULTI_MATCH_HPP
#define MULTI_MATCH_HPP

#include "match.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include <optional>

namespace REMatch {
using namespace rematch;

class MultiMatch {
 public:
  MultiMatch(
      ExtendedMapping extended_mapping,
      std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
      std::shared_ptr<Document> document);

  std::vector<Span> spans(int variable_id);
  std::vector<Span> spans(std::string variable_name);
  std::vector<std::string> groups(int variable_id);
  std::vector<std::string> groups(std::string variable_name);
  MultiMatch submatch(Span span);
  bool empty();

  bool operator==(const MultiMatch& other) const;

 private:
  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  ExtendedMapping extended_mapping_;
  std::optional<std::map<int, std::vector<Span>>> mapping_cache_ = std::nullopt;
};

}  // namespace REMatch

#endif
