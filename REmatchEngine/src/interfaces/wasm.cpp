// Make VSCode Linter ignore the include error
#include <string>
#include <utility>

#include "regex/regex.hpp"
#include "regex/regex_options.hpp"
#include "match.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS() {
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
    .constructor<std::string>()
    .function("findIter", &rematch::RegEx::findIter)
    .function("find", &rematch::RegEx::find)
    .function("pattern", &rematch::RegEx::pattern)
    ;
}

#endif