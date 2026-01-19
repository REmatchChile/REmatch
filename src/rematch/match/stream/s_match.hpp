#pragma once

#include "evaluation/stream.hpp"
#include "match/match.hpp"
#include "mediator/mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {

class SMatch : public Match {
 public:
  SMatch(std::unique_ptr<mediator::Mapping> mapping,
         std::shared_ptr<VariableCatalog> variable_catalog, std::shared_ptr<Stream> stream);

  ~SMatch() override;

  SMatch(const SMatch& other);
  SMatch& operator=(const SMatch& other);

  SMatch(SMatch&& other) noexcept;
  SMatch& operator=(SMatch&& other) noexcept;

  int64_t start(const std::string& variable_name) const override;
  int64_t start(uint_fast32_t variable_id) const override;

  int64_t end(const std::string& variable_name) const override;
  int64_t end(uint_fast32_t variable_id) const override;

  Span span(const std::string& variable_name) const override;
  Span span(uint_fast32_t variable_id) const override;

  std::string group(const std::string& variable_name) const override;
  std::string group(uint_fast32_t variable_id) const override;

  std::vector<std::string> variables() const override;

  std::map<std::string, Span> groupdict() const override;

  std::string to_string() const override;

  bool empty() const override;

  std::unique_ptr<Match> clone() const override { return std::make_unique<SMatch>(*this); }

 private:
  std::unique_ptr<mediator::Mapping> mapping_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::shared_ptr<Stream> stream;
};

}  // namespace REmatch
