#include "filtering_module/segment_checker.hpp"
#include "mediator/mediator.hpp"

namespace REmatch {

class StreamCheckLblMediator : public Mediator {
 public:
  StreamCheckLblMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                         std::unique_ptr<SegmentCheckerStream> checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  std::shared_ptr<Stream> stream;
  std::unique_ptr<SegmentCheckerStream> checker;
  bool has_returned = false;
};

}  // namespace REmatch
