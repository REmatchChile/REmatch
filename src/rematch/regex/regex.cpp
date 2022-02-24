#include "regex.hpp"

#include "parse/parser.hpp"
// #include "evaluation/evaluator.hpp"
#include "automata/nfa/lva.hpp"
#include "automata/nfa/sva.hpp"

namespace rematch {

RegEx::RegEx(const std::string &pattern, rematch::RegExOptions rgx_opts)
    : pattern_(pattern),
      flags_(parseFlags(rgx_opts)) {

  std::unique_ptr<LogicalVA> VA = regex2LVA(pattern);

  vfactory_ = VA->varFactory();

  eVA_ = std::make_unique<ExtendedVA>(*VA);
  sVA_ = std::make_unique<SearchVA>(*VA);
}

// Explicitly declared here for correct use of unique_ptr later
RegEx::~RegEx() {}

MatchIterator RegEx::findIter(std::shared_ptr<Document> d, Anchor a) {
  // Evaluator* eval;
  // std::shared_ptr<StrDocument> strd = std::static_pointer_cast<StrDocument>(d);
  // eval = new EarlyOutputFilterEvaluatorNewV2(*this, strd, a);
  // return MatchIterator(eval);
  // if (flags_ & kEarlyOutput) {
  //   if (flags_ & kLineByLine) {
  //     eval = new EarlyOutputLineEvaluator(*this, d);
  //   } else {
  //     // if(dman_->nfa().is_dfa_searchable()) {
  //       std::shared_ptr<StrDocument> strd = std::static_pointer_cast<StrDocument>(d);
  //       eval = new EarlyOutputFilterEvaluator(*this, strd);
  //     // } else {
  //     //   eval = new EarlyOutputEvaluator(*this, d);
  //     // }
  //   }
  // } else {
  //   if (flags_ & kLineByLine) {
  //     eval = new LineEvaluator(*this, d);
  //   } else {
  //     eval = new NormalEvaluatorNew(*this, d);
  //   }
  // }
  // return MatchIterator(eval);
}

Match_ptr RegEx::find(const std::string &text) {
  // auto document = std::make_shared<StrDocument>(text);
  // return EarlyOutputEvaluator(*this, document).next();
}

uint8_t RegEx::parseFlags(rematch::RegExOptions rgx_opts) {
  uint8_t ret =  rgx_opts.multi_line()    * kMultiLine    |
                 rgx_opts.line_by_line()  * kLineByLine   |
                 rgx_opts.dot_nl()        * kDotNL        |
                 rgx_opts.early_output()  * kEarlyOutput  |
                 rgx_opts.save_anchors()  * kSaveAnchors;
  return ret;
}

} // end namespace rematch




