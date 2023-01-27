#ifndef LIBRARY_INTERFACE__MATCH_HPP
#define LIBRARY_INTERFACE__MATCH_HPP

#include <cstdlib>
#include <utility>
#include <string_view>

namespace REMatch {

inline namespace library_interface {

  using Span = std::pair<size_t, size_t>;

  class Match {

    private:
      Span span;
      std::string_view document;

    public:
      Match(Span span, std::string_view document);

      Span get_span() const;
      std::string_view to_string() const;
  };
} // end namespace library_interface

} // end namespace rematch

#endif
