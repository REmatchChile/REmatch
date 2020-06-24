#include "rematch.hpp"

#include <sstream>

#include "regex/regex.hpp"
#include "eval.hpp"

using namespace std;

namespace rematch {

std::unique_ptr<RegEx> compile(string pattern) {
  return std::make_unique<RegEx>(pattern);
}
std::unique_ptr<RegEx> compile(string pattern, RegExOptions opt) {
  return std::make_unique<RegEx>(pattern);
}

std::unique_ptr<Match> find(string pattern, string &text) {
  RegEx compiled_rgx(pattern);
  stringstream ss(text);

  Evaluator eval(compiled_rgx, ss, Evaluator::kEarlyOutput);

  return eval.next();
}

// std::vector<Match> findAll(string pattern, string &text) {
//   RegEx rgx(pattern);
//   stringstream ss(text);

//   Evaluator eval(rgx, ss);


// }

} // namespace rematch


