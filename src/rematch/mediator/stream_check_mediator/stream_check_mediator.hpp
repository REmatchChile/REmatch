#include "filtering_module/segment_checker.hpp"
#include "mediator/mediator.hpp"

namespace REmatch {

class StreamCheckMediator : public Mediator {
 public:
  StreamCheckMediator(QueryData& query_data, std::unique_ptr<SegmentCheckerStream> checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  std::unique_ptr<SegmentCheckerStream> checker;
  bool has_returned = false;
};

}  // namespace REmatch
