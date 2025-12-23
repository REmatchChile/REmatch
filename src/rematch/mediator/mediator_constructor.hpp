#pragma once

#include "filtering_module/document_segment.hpp"
#include "filtering_module/segment_identificator_stream.hpp"
#include "finditer_mediator/finditer_lbl_mediator.hpp"
#include "finditer_mediator/finditer_mediator.hpp"
#include "findone_mediator/findone_lbl_mediator.hpp"
#include "findone_mediator/findone_mediator.hpp"
#include "mediator.hpp"
#include "multi_finditer_mediator/multi_finditer_lbl_mediator.hpp"
#include "multi_finditer_mediator/multi_finditer_mediator.hpp"
#include "multi_findone_mediator/multi_findone_lbl_mediator.hpp"
#include "multi_findone_mediator/multi_findone_mediator.hpp"
#include "stream_mediator/stream_lbl_mediator.hpp"
#include "stream_mediator/stream_mediator.hpp"

namespace REmatch {

class MediatorConstructor {
 public:
  static std::unique_ptr<Mediator> create_finditer_mediator(QueryData& query_data,
                                                            std::shared_ptr<Document> document) {
    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      auto line_splitter = std::make_unique<LineSplitterStr>(document);
      auto segment_checker = std::make_unique<SegmentChecker>(std::move(search_dfa), document);
      return std::make_unique<FinditerLblMediator>(query_data, document, std::move(line_splitter),
                                                   std::move(segment_checker));
    }

    std::unique_ptr<SegmentIdentificatorBase> segment_identificator;

    if (query_data.logical_va.has_useful_anchors()) {
      // this segment identificator returns the span [0, doc_size + 1]
      segment_identificator = std::make_unique<DocumentSegment>(document);
    } else {
      // this segment identificator returns segments that contain outputs
      segment_identificator =
          std::make_unique<SegmentIdentificator>(std::move(search_dfa), document);
    }

    return std::make_unique<FinditerMediator>(query_data, document,
                                              std::move(segment_identificator));
  }

  static std::unique_ptr<Mediator> create_findone_mediator(QueryData& query_data,
                                                           std::shared_ptr<Document> document) {
    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      auto line_splitter = std::make_unique<LineSplitterStr>(document);
      auto segment_checker = std::make_unique<SegmentChecker>(std::move(search_dfa), document);
      return std::make_unique<FindoneLblMediator>(query_data, document, std::move(line_splitter),
                                                  std::move(segment_checker));

    } else {
      // this segment identificator returns segments that contain outputs
      auto segment_identificator =
          std::make_unique<SegmentIdentificator>(std::move(search_dfa), document);

      return std::make_unique<FindoneMediator>(query_data, document,
                                               std::move(segment_identificator));
    }
  }

  static std::unique_ptr<MultiMediator> create_multi_finditer_mediator(
      QueryData& query_data, std::shared_ptr<Document> document) {
    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      auto line_splitter = std::make_unique<LineSplitterStr>(document);
      auto segment_checker = std::make_unique<SegmentChecker>(std::move(search_dfa), document);
      return std::make_unique<MultiFinditerLblMediator>(
          query_data, document, std::move(line_splitter), std::move(segment_checker));
    }

    std::unique_ptr<SegmentIdentificatorBase> segment_identificator;

    if (query_data.logical_va.has_useful_anchors()) {
      // this segment identificator returns the span [0, doc_size + 1]
      segment_identificator = std::make_unique<DocumentSegment>(document);
    } else {
      // this segment identificator returns segments that contain outputs
      segment_identificator =
          std::make_unique<SegmentIdentificator>(std::move(search_dfa), document);
    }

    return std::make_unique<MultiFinditerMediator>(query_data, document,
                                                   std::move(segment_identificator));
  }

  static std::unique_ptr<MultiMediator> create_multi_findone_mediator(
      QueryData& query_data, std::shared_ptr<Document> document) {
    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      auto line_splitter = std::make_unique<LineSplitterStr>(document);
      auto segment_checker = std::make_unique<SegmentChecker>(std::move(search_dfa), document);
      return std::make_unique<MultiFindoneLblMediator>(
          query_data, document, std::move(line_splitter), std::move(segment_checker));

    } else {
      // this segment identificator returns segments that contain outputs
      auto segment_identificator =
          std::make_unique<SegmentIdentificator>(std::move(search_dfa), document);

      return std::make_unique<MultiFindoneMediator>(query_data, document,
                                                    std::move(segment_identificator));
    }
  }

  static std::unique_ptr<Mediator> create_stream_mediator(QueryData& query_data,
                                                          std::shared_ptr<Stream> stream) {

    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {

      auto line_splitter = std::make_unique<LineSplitterStream>(stream);

      auto segment_checker = std::make_unique<SegmentCheckerStream>(std::move(search_dfa), stream);

      return std::make_unique<StreamLblMediator>(query_data, stream, std::move(line_splitter),
                                                 std::move(segment_checker));

    } else {
      auto segment_identificator =
          std::make_unique<SegmentIdentificatorStream>(std::move(search_dfa), stream);

      return std::make_unique<StreamMediator>(query_data, stream, std::move(segment_identificator));
    }
  }

  // TODO: use a segment checker instead of segment identificator if line by line is used
  static std::unique_ptr<Mediator> create_stream_findone_mediator(QueryData& query_data,
                                                                  std::shared_ptr<Stream> stream) {

    DFAStateLimitChecker dfa_states_checker(query_data.max_amount_of_states);
    auto search_dfa = std::make_unique<SearchDFA>(query_data.logical_va, dfa_states_checker);

    if ((query_data.flags & Flags::LINE_BY_LINE) != Flags::NONE) {

      auto line_splitter = std::make_unique<LineSplitterStream>(stream);

      auto segment_checker = std::make_unique<SegmentCheckerStream>(std::move(search_dfa), stream);

      return std::make_unique<StreamLblMediator>(query_data, stream, std::move(line_splitter),
                                                 std::move(segment_checker));

    } else {
      auto segment_identificator =
          std::make_unique<SegmentIdentificatorStream>(std::move(search_dfa), stream);

      return std::make_unique<StreamMediator>(query_data, stream, std::move(segment_identificator));
    }
  }
};
}  // namespace REmatch
