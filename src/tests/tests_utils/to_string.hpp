

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "REmatch/span.hpp"

namespace Catch {
template <>
struct StringMaker<Span> {
  static std::string convert(Span const& span) {
    return "[" + std::to_string(span.first) + "," +
           std::to_string(span.second) + ">";
  }
};

template <>
struct StringMaker<std::vector<Span>> {
  static std::string convert(std::vector<Span> const& spans) {
    std::string res;
    bool first = true;
    for (auto& span : spans) {
      if (first) {
        first = false;
        res += StringMaker<Span>::convert(span);
      } else {
        res += ",";
        res += StringMaker<Span>::convert(span);
      }
    }
    return res;
  }
};
}  // namespace Catch
