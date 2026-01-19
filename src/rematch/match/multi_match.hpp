#pragma once

#include <memory>
#include <string>
#include <vector>

#include "REmatch/span.hpp"

namespace REmatch {

class MultiMatch {
 public:
  virtual ~MultiMatch() = default;

  virtual std::vector<Span> spans(const std::string& variable_name) const = 0;
  virtual std::vector<Span> spans(uint_fast32_t variable_id) const = 0;
  virtual std::vector<std::string> groups(const std::string& variable_name) const = 0;
  virtual std::vector<std::string> groups(uint_fast32_t variable_id) const = 0;
  virtual bool empty() const = 0;
  virtual std::unique_ptr<MultiMatch> submatch(Span span) const = 0;
  virtual std::vector<std::string> variables() const = 0;
  virtual std::string to_string() const = 0;
  virtual std::unique_ptr<MultiMatch> clone() const = 0;
};

}  // namespace REmatch
