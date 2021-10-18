#include "regex.hpp"

#include "parse/parser.hpp"
#include "evaluation/evaluator.hpp"
#include "automata/nfa/lva.hpp"

namespace rematch {

RegEx::RegEx(const std::string &pattern, rematch::RegExOptions rgx_opts)
    : pattern_(pattern),
      flags_(parseFlags(rgx_opts)) {

  std::unique_ptr<LogicalVA> VA = regex2LVA(pattern);

  std::unique_ptr<LogicalVA> rawVA = std::make_unique<LogicalVA>(*VA);
  rawVA->adapt_capture_jumping();

  auto eVA = std::make_unique<ExtendedVA>(*VA);
  auto raw_eVA = std::make_unique<ExtendedVA>(*rawVA);

  dman_ = std::make_unique<DetManager>(eVA);
  raw_dman_ = std::make_unique<DetManager>(raw_eVA);
}

// Explicitly declared here for correct use of unique_ptr later
RegEx::~RegEx() {}


MatchIterator RegEx::findIter(std::shared_ptr<Document> d) {
  Evaluator* eval;
  std::shared_ptr<StrDocument> strd = std::static_pointer_cast<StrDocument>(d);
  eval = new EarlyOutputFilterEvaluatorNewV2(*this, strd);
  return MatchIterator(eval);
  if (flags_ & kEarlyOutput) {
    if (flags_ & kLineByLine) {
      eval = new EarlyOutputLineEvaluator(*this, d);
    } else {
      if(dman_->nfa().is_dfa_searchable()) {
        std::shared_ptr<StrDocument> strd = std::static_pointer_cast<StrDocument>(d);
        eval = new EarlyOutputFilterEvaluator(*this, strd);
      } else {
        eval = new EarlyOutputEvaluator(*this, d);
      }
    }
  } else {
    if (flags_ & kLineByLine) {
      eval = new LineEvaluator(*this, d);
    } else {
      eval = new NormalEvaluatorNew(*this, d);
    }
  }
  return MatchIterator(eval);
}


Match_ptr RegEx::find(const std::string &text) {
  auto document = std::make_shared<StrDocument>(text);
  return EarlyOutputEvaluator(*this, document).next();
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
  return dman_->dfa().size();
}
size_t RegEx::nfa_counter() {
  return dman_->nfa().size();
}
size_t RegEx::mdfa_counter() {
  return dman_->mdfa().size();
}
size_t RegEx::miss_counter() {
  return 0;
}




} // end namespace rematch




