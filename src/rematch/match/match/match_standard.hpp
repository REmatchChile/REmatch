#pragma once

#include "evaluation/document.hpp"
#include "match/match/match.hpp"
#include "mediator/mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch::internal {

class MatchStandard : public Match {

 public:
  MatchStandard(std::unique_ptr<mediator::Mapping> mapping,
                std::shared_ptr<parsing::VariableCatalog> variable_catalog,
                std::shared_ptr<Document> document);

  // Copy and move constructors
  MatchStandard(const MatchStandard& other);
  MatchStandard& operator=(const MatchStandard& other);

  MatchStandard(MatchStandard&& other) noexcept;
  MatchStandard& operator=(MatchStandard&& other) noexcept;

  ~MatchStandard() override;

  int64_t start(const std::string& variable_name) const override;
  int64_t start(uint_fast32_t variable_id) const override;

  int64_t end(const std::string& variable_name) const override;
  int64_t end(uint_fast32_t variable_id) const override;

  Span span(const std::string& variable_name) const override;
  Span span(uint_fast32_t variable_id) const override;

  std::string group(const std::string& variable_name) const override;
  std::string group(uint_fast32_t variable_id) const override;

  std::map<std::string, Span> groupdict() const override;

  std::vector<std::string> variables() const override;

  bool empty() const override;

  std::string to_string() const override;

  std::unique_ptr<Match> clone() const override { return std::make_unique<MatchStandard>(*this); }

 private:
  std::unique_ptr<mediator::Mapping> mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
};

}  // namespace REmatch
