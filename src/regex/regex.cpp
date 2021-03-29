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


MatchIterator RegEx::findIter(std::shared_ptr<Document> d) {
  auto eval = new Evaluator(*this, d, Evaluator::kAllFlags & flags_);
  return MatchIterator(eval);
}


Match_ptr RegEx::find(const std::string &text) {
  auto document = std::make_shared<StrDocument>(text);
  return Evaluator(*this, document, flags_ & Evaluator::kEarlyOutput).next();
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
  return 0;
}

size_t RegEx::reading_counter() const {
  return 0;
}

size_t RegEx::direct_counter() const {
  return 0;
}

size_t RegEx::single_counter() const {
  return 0;
}
size_t RegEx::direct_single_counter() const {
  return 0;
}
size_t RegEx::direct_multi_counter() const {
  return 0;
}
size_t RegEx::multi_counter() const {
  return 0;
}
size_t RegEx::empty_counter() const {
  return 0;
}
size_t RegEx::det_counter() const {
  return 0;
}
size_t RegEx::dfa_counter() {
  return dman_.dfa().size();
}
size_t RegEx::nfa_counter() {
  return dman_.nfa().size();
}
size_t RegEx::mdfa_counter() {
  return dman_.mdfa().size();
}
size_t RegEx::miss_counter() {
  return 0;
}




} // end namespace rematch




