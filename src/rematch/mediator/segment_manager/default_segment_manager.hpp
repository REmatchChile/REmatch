#ifndef DEFAULT_SEGMENT_MANAGER_HPP
#define DEFAULT_SEGMENT_MANAGER_HPP

#include "segment_manager.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace rematch {

class DefaultSegmentManager : public SegmentManager {
 public:
  DefaultSegmentManager(std::string_view document);

  std::unique_ptr<Span> next() override;

 private:
  std::string document_;
  bool already_read_;
};

}

#endif
