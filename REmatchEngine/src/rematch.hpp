#ifndef REMATCH_HPP
#define REMATCH_HPP

#include <string>

#include "regex/regex.hpp"
#include "match.hpp"

namespace rematch {

enum Anchor {
  UNANCHORED,
  ANCHOR_START,
  ANCHOR_BOTH
};

std::unique_ptr<RegEx> compile(std::string pattern, RegExOptions opt);
std::unique_ptr<RegEx> compile(std::string pattern);

std::unique_ptr<Match> find(std::string pattern, std::string &text);
std::unique_ptr<Match> find(std::string pattern, std::string &text, Anchor anchor);

std::vector<Match> findAll(std::string pattern, std::string &text);

std::unique_ptr<Match> findIter(std::string pattern, std::string &text);

bool doFind(std::string pattern, std::string &text,
            Anchor anchor, RegExOptions opt);

} // namespace rematch


#endif // REMATCH_HPP