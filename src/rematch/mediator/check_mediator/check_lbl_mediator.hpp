#include "filtering_module/segment_checker.hpp"
#include "mediator/mediator.hpp"

namespace REmatch {

class CheckLblMediator : public Mediator {
 public:
  CheckLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                   std::unique_ptr<SegmentChecker> checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  std::shared_ptr<Document> document;
  std::unique_ptr<SegmentChecker> checker;
  bool has_returned = false;
};

}  // namespace REmatch
