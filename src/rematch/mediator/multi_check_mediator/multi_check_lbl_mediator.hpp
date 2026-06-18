#include "filtering_module/segment_checker.hpp"
#include "mediator/mediator.hpp"
#include "mediator/multi_mediator.hpp"

namespace REmatch {

class MultiCheckLblMediator : public MultiMediator {
 public:
  MultiCheckLblMediator(std::shared_ptr<Document> document,
                        std::unique_ptr<SegmentChecker> checker);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  std::shared_ptr<Document> document;
  std::unique_ptr<SegmentChecker> checker;
  const output_enumeration::Mapping mapping;
  bool has_returned = false;
};

}  // namespace REmatch
