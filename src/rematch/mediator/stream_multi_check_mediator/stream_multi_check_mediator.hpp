#include "filtering_module/segment_checker.hpp"
#include "mediator/multi_mediator.hpp"

namespace REmatch {

class StreamMultiCheckMediator : public MultiMediator {
 public:
  explicit StreamMultiCheckMediator(std::unique_ptr<SegmentCheckerStream> checker);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  std::unique_ptr<SegmentCheckerStream> checker;
  const output_enumeration::Mapping mapping;
  bool has_returned = false;
};

}  // namespace REmatch
