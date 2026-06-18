#include "filtering_module/segment_checker.hpp"
#include "mediator/mediator.hpp"
#include "mediator/multi_mediator.hpp"

namespace REmatch {

class StreamMultiCheckLblMediator : public MultiMediator {
 public:
  explicit StreamMultiCheckLblMediator(std::shared_ptr<Stream> stream,
                                       std::unique_ptr<SegmentCheckerStream> checker);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  std::shared_ptr<Stream> stream;
  std::unique_ptr<SegmentCheckerStream> checker;
  const output_enumeration::Mapping mapping;
  bool has_returned = false;
};

}  // namespace REmatch
