/*
Author:
    nicovsj
Description:
    Module defining the structs used at parsing-time.
*/

#ifndef AST_HPP
#define AST_HPP

#include <set>
#include <vector>
#include <string>
#include <iostream>

#include <boost/fusion/adapted.hpp>
#include <boost/variant.hpp>     // for tree nodes
#include <boost/optional.hpp>    // for multiplicity upperbound

namespace rematch {

enum class SpecialCode {
  kNoSpecial = 0,
  kAnyChar,
  kAnyDigit,
  kAnyWord,
  kAnyWhiteSpace,
};

enum class AssertionCode {
  kNoAssertion = 0,
  kStartAnchor,
  kEndAnchor,
  kWordBoundary,
  kNoWordBoundary
};

namespace ast {
struct charset {
    /* Struct for class of char information containment */

    bool negated;  // Is a negated charset ?

    using range = std::tuple<char, char>;
    using element = boost::variant<char, range>;

    std::set<element> elements;
};

// Recursive structs
struct parenthesis;
struct assignation;

// An assertion that does not consume input (e.g. ^, $, \b, \B)
struct assertion {
  assertion() = default;

  assertion(AssertionCode b): code_(b) {}
  AssertionCode code_;
};

// Special charclasses (e.g. ".", "\d", "\W")
struct special {
  special() = default;

  special(SpecialCode c, bool not_negated)
    : code_(c), not_negated_(not_negated) {}

  SpecialCode code_;
  bool not_negated_;
};


using atom = boost::variant<charset, assertion, char, special>;

using group =  boost::variant<
    boost::recursive_wrapper<parenthesis>,
    boost::recursive_wrapper<assignation>,
    atom>;

struct repetition {
    repetition(int s, int e): start_(s), end_(e) {}

    repetition(): start_(-1), end_(-1) {}

    int start_;
    int end_;
};

// Iter
struct iter {
    group expr;
    std::vector<repetition> repetitions;
};

// Concat and altern
using concat = std::vector<iter>;
using altern = std::vector<concat>;

struct parenthesis {
    altern root;

    parenthesis() = default;

    parenthesis(altern root): root(std::move(root)) {}

    friend std::ostream& operator<<(std::ostream& os, const parenthesis& p);
};

struct assignation {
    std::string var;

    altern root;

    assignation() = default;

    assignation(std::string var, altern root): var(var), root(std::move(root)) {}
};

} // end namespace ast
} // end namespace rematch

BOOST_FUSION_ADAPT_STRUCT(rematch::ast::charset,
        (bool, negated)
        (std::set<rematch::ast::charset::element>, elements))

BOOST_FUSION_ADAPT_STRUCT(rematch::ast::iter,
        (rematch::ast::group, expr)
        (std::vector<rematch::ast::repetition>, repetitions))

BOOST_FUSION_ADAPT_STRUCT(rematch::ast::assignation,
        (std::string, var)
        (rematch::ast::altern, root))

#endif