#ifndef REGEX__REGEX_OPTIONS_HPP
#define REGEX__REGEX_OPTIONS_HPP

#include "anchors.hpp"

namespace rematch {

class RegExOptions {
 public:
  RegExOptions()
      : multi_line_(false),
        line_by_line_(false),
        dot_nl_(false),
        anchors_(Anchor::kBothAnchors),
        early_output_(false),
        save_anchors_(false) {}

  bool multi_line() const {return multi_line_;}
  void set_multi_line(bool b) {multi_line_ = b;}

  bool line_by_line() const {return line_by_line_;}
  void set_line_by_line(bool b) {line_by_line_ = b;}

  bool dot_nl() const {return dot_nl_;}
  void set_dot_nl(bool b) {dot_nl_ = b;}

  bool start_anchor() const {return anchors_ & Anchor::kSingleAnchor;}
  void set_start_anchor(bool b) {
    if(b)
      anchors_ |= Anchor::kSingleAnchor;
    else
      anchors_ &= ~Anchor::kSingleAnchor;
  }

  bool end_anchor() const {return anchors_ & Anchor::kFinalAnchor;}
  void set_end_anchor(bool b) {
    if(b)
      anchors_ |= Anchor::kFinalAnchor;
    else
      anchors_ &= ~Anchor::kFinalAnchor;
  }

  Anchor anchors() const {
    Anchor check = static_cast<Anchor>(anchors_);
    return check;
    }

  bool early_output() const {return early_output_;}
  void set_early_output(bool b) {early_output_ = b;}

  bool save_anchors() const {return save_anchors_;}
  void set_save_anchors(bool b) {save_anchors_ = b;}

 private:

  bool multi_line_;
  bool line_by_line_;
  bool dot_nl_;
  int anchors_;
  bool early_output_;
  bool save_anchors_;
}; // end class RegExOptions

} // end namespace rematch

#endif // REGEX__REGEX_OPTIONS_HPP