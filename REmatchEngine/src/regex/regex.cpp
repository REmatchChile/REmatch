#include "regex.hpp"
#include "parser/parser.hpp"
#include "eval.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace rematch {

RegEx::RegEx(std::string pattern, rematch::RegExOptions rgx_opts)
    : pattern_(pattern),
      dman_(pattern),
      raw_dman_(pattern, 1),
      flags_(parseFlags(rgx_opts)) {}

// Explicitly declared here for correct use of unique_ptr later
RegEx::~RegEx() {}

std::string RegEx::uniformGenerate(uint32_t n) {
  if(!full_dfa_) {
    dman_.computeFullDetAutomaton();
    full_dfa_ = true;
  }

  return dman_.uniformSample(n);
}


std::unique_ptr<Match> RegEx::findIter(std::istream &text, uint8_t eval_flags) {
  if(eval_ == nullptr) {
    eval_ = std::make_unique<Evaluator>(*this, text, Evaluator::kAllFlags & flags_);
  }
  return eval_->next();
}


std::unique_ptr<Match> RegEx::find(std::istream &text, uint8_t eval_flags) {
  return Evaluator(*this, text, eval_flags & Evaluator::kEarlyOutput).next();
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


// Make VSCode Linter ignore the include error
#ifdef __EMSCRIPTEN__

EMSCRIPTEN_BINDINGS(my_class_example) {
  emscripten::register_vector<std::string>("VectorStr");

  emscripten::value_array<std::pair<size_t, size_t>>("pair")
     .element(&std::pair<size_t, size_t>::first)
     .element(&std::pair<size_t, size_t>::second)
     ;

  emscripten::class_<rematch::Match>("Match")
    .constructor<>()
    .function("span", &rematch::Match::span)
    .function("start", &rematch::Match::start)
    .function("end", &rematch::Match::end)
    .function("group", &rematch::Match::group)
    .function("variables", &rematch::Match::variables)
    ;

  emscripten::class_<rematch::RegExOptions>("RegExOptions")
    .constructor<>()
    .property("multi_line", &rematch::RegExOptions::multi_line, &rematch::RegExOptions::set_multi_line)
    .property("line_by_line", &rematch::RegExOptions::line_by_line, &rematch::RegExOptions::set_line_by_line)
    .property("dot_nl", &rematch::RegExOptions::dot_nl, &rematch::RegExOptions::set_dot_nl)
    .property("early_output", &rematch::RegExOptions::early_output, &rematch::RegExOptions::set_early_output)
    .property("save_anchors", &rematch::RegExOptions::save_anchors, &rematch::RegExOptions::set_save_anchors)
    ;

  emscripten::class_<rematch::RegEx>("RegEx")
    .constructor<std::string, rematch::RegExOptions>()
    .function("findIter", &rematch::RegEx::findIter)
    .function("find", &rematch::RegEx::find)
    .function("pattern", &rematch::RegEx::pattern)
    ;
}

#endif

