#include <sys/resource.h>

#include "regex.hpp"
#include "parser/parser.hpp"
#include "eval.hpp"
#include "document.hpp"
#include "util.hpp"

namespace rematch {

RegEx::RegEx(std::string pattern, rematch::RegExOptions rgx_opts)
    : pattern_(pattern),
      dman_(pattern),
      raw_dman_(pattern, 1),
      flags_(parseFlags(rgx_opts)) {}

// Explicitly declared here for correct use of unique_ptr later
RegEx::~RegEx() {}

void RegEx::feed(const std::string& text) {
  if(!doc_) {
    doc_ = std::make_unique<ChunkDocument>();
  }
  ChunkDocument *chunk_doc = dynamic_cast<ChunkDocument*>(doc_.get());
  chunk_doc->feed(text);
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, *doc_, Evaluator::kAllFlags & flags_);
  }
}

Match_ptr RegEx::internalFindIter() {
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, *doc_, Evaluator::kAllFlags & flags_);
  }
  return eval_->next();
}

std::string RegEx::uniformGenerate(uint32_t n) {
  if(!full_dfa_) {
    dman_.computeFullDetAutomaton();
    full_dfa_ = true;
  }
  return dman_.uniformSample(n);
}

Match_ptr RegEx::findIter(const std::string &text) {
  if(!doc_) {
    doc_ = std::make_unique<StrDocument>(text);
    eval_ = std::make_unique<Evaluator>(*this, *doc_, Evaluator::kAllFlags & flags_);
  } else if(doc_->data() != text.data() || doc_->size() != text.size()) {
    doc_ = std::make_unique<StrDocument>(text);
    eval_ = std::make_unique<Evaluator>(*this, *doc_, Evaluator::kAllFlags & flags_);
  }
  return eval_->next();
}

Match_ptr RegEx::findIterFP(std::istream &is) {
  ChunkDocument doc;
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, doc, Evaluator::kAllFlags & flags_);
    std::getline(is, buffer_);
    doc.feed(std::as_const(buffer_));
  }

  Match_ptr res = eval_->next();

  if(!res && !std::getline(is, buffer_)){
    doc.feed(std::as_const(buffer_));
    res = eval_->next();
  }

  return res;
}


Match_ptr RegEx::find(const std::string &text) {
  StrDocument doc(text);
  return Evaluator(*this, doc, flags_ & Evaluator::kEarlyOutput).next();
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

} // end namespace rematch




