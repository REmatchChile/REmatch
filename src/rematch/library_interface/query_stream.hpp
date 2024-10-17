#include <memory>
#include <string>
#include <vector>

#include "evaluation/document.hpp"
#include "flags.hpp"
#include "library_interface/match_iterator_stream.hpp"
#include "library_interface/reader.hpp"
#include "match.hpp"
#include "match_iterator.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/mediator/stream_mediator.hpp"
#include "mediator/output_checker.hpp"
#include "query_data/query_data_utils.hpp"

using namespace rematch;

namespace REMatch {

class QueryStream {
 public:
  explicit QueryStream(const std::string& pattern, Flags flags = Flags())
      : flags_(flags), query_data_(rematch::get_query_data(pattern, flags)) {}

  std::unique_ptr<Match> find(std::shared_ptr<Reader> reader) {
    auto stream = std::make_shared<Stream>(reader);
    auto mediator =
        std::make_unique<StreamMediator>(query_data_, stream, flags_);

    rematch::mediator::Mapping* mapping = mediator->next();
    if (mapping != nullptr) {
      return std::make_unique<Match>(*mapping, query_data_.variable_catalog, std::make_shared<Document>("0"));
    }

    std::cout << mapping->get_span_of_variable("x") << std::endl;
    return nullptr;
  }

  std::unique_ptr<MatchIteratorStream> finditer(std::shared_ptr<Reader> reader) {
    return std::make_unique<MatchIteratorStream>(query_data_, reader, flags_);
  }

  Flags flags_;
  rematch::QueryData query_data_;
};
}  // namespace REMatch
