#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "REmatch/span.hpp"

namespace REmatch {

class Match {
 public:
  virtual ~Match() = default;

  virtual int64_t start(const std::string& variable_name) const = 0;
  virtual int64_t start(uint_fast32_t variable_id) const = 0;
  virtual int64_t end(const std::string& variable_name) const = 0;
  virtual int64_t end(uint_fast32_t variable_id) const = 0;
  virtual Span span(const std::string& variable_name) const = 0;
  virtual Span span(uint_fast32_t variable_id) const = 0;
  virtual std::string group(const std::string& variable_name) const = 0;
  virtual std::string group(uint_fast32_t variable_id) const = 0;
  virtual bool empty() const = 0;
  virtual std::map<std::string, Span> groupdict() const = 0;
  virtual std::vector<std::string> variables() const = 0;
  virtual std::string to_string() const = 0;
  virtual std::unique_ptr<Match> clone() const = 0;
};

}  // namespace REmatch
