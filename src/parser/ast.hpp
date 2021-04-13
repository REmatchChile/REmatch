#ifndef AST_HPP
#define AST_HPP

#include <set>
#include <vector>
#include <string>
#include <boost/tuple/tuple.hpp> // for charset
#include <boost/fusion/adapted.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/variant.hpp>     // for tree nodes
#include <boost/optional.hpp>    // for multiplicity upperbound

namespace rematch {
namespace ast {

struct charset {
    /* Struct for class of char information containment */

    bool negated;  // Is a negated charset ?

    using range = std::tuple<char, char>;
    using element = boost::variant<char, range>;

    std::set<element> elements;
};

// Special classes
struct anychar {};
struct anydigit {};
struct nondigit {};
struct anyword {};
struct nonword {};
struct anywhitespace {};

// Recursive structs
struct parenthesis;
struct assignation;

// Atom
typedef boost::variant<charset, char, anychar, anydigit, nondigit,
                        anyword, nonword, anywhitespace> atom;
// Group
typedef boost::variant<
    boost::recursive_wrapper<parenthesis>,
    boost::recursive_wrapper<assignation>,
    atom> group;

// Iter
struct iter {
    group expr;
    std::vector<char> repetitions;
};

// Concat and altern
using concat = std::vector<iter>;
using altern = std::vector<concat>;

struct parenthesis {
    altern root;

    parenthesis() = default;

    parenthesis(altern root): root(std::move(root)) {}
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
        (std::vector<char>, repetitions))

BOOST_FUSION_ADAPT_STRUCT(rematch::ast::assignation,
        (std::string, var)
        (rematch::ast::altern, root))

#endif