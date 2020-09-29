#include "regex.hpp"
#include "parser/parser.hpp"
#include "eval.hpp"

namespace rematch {

RegEx::RegEx(std::string pattern, rematch::RegExOptions rgx_opts)
    : pattern_(pattern),
      dman_(pattern),
      raw_dman_(pattern, 1),
      flags_(parseFlags(rgx_opts)) {}

// Explicitly declared here for correct use of unique_ptr later
RegEx::~RegEx() {}

// std::string RegEx::uniformGenerate(uint32_t n) {
//   if(!full_dfa_) {
//     dman_.computeFullDetAutomaton();
//     full_dfa_ = true;
//   }
//   return dman_.uniformSample(n);
// }


Match_ptr RegEx::findIter(const std::string &text) {
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, text, Evaluator::kAllFlags & flags_);
  }
  return eval_->next();
}

Match_ptr RegEx::findIter(std::istream &is) {
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, is, Evaluator::kAllFlags & flags_);
  }
  return eval_->next();
}


Match_ptr RegEx::find(const std::string &text) {
  return Evaluator(*this, text, flags_ & Evaluator::kEarlyOutput).next();
}


uint8_t RegEx::parseFlags(rematch::RegExOptions rgx_opts) {
  uint8_t ret =  rgx_opts.multi_line()    * kMultiLine    |
                 rgx_opts.line_by_line()  * kLineByLine   |
                 rgx_opts.dot_nl()        * kDotNL        |
                 rgx_opts.early_output()  * kEarlyOutput  |
                 rgx_opts.save_anchors()  * kSaveAnchors;
  return ret;
}

size_t RegEx::capture_counter() const {
  return eval_->capture_counter_;
}

size_t RegEx::reading_counter() const {
  return eval_->reading_counter_;
}

size_t RegEx::direct_counter() const {
  return eval_->direct_c_;
}

size_t RegEx::single_counter() const {
  return eval_->single_c_;
}
size_t RegEx::direct_single_counter() const {
  return eval_->direct_single_c_;
}
size_t RegEx::direct_multi_counter() const {
  return eval_->direct_multi_c_;
}
size_t RegEx::multi_counter() const {
  return eval_->multi_c_;
}
size_t RegEx::empty_counter() const {
  return eval_->empty_c_;
}
size_t RegEx::det_counter() const {
  return eval_->det_c_;
}
size_t RegEx::dfa_counter() {
  return dman_.DFA().size();
}
size_t RegEx::nfa_counter() {
  return dman_.NFA().size();
}



} // end namespace rematch




