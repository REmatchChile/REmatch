// #define BOOST_SPIRIT_DEBUG
#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_UNICODE

// Prevent ambiguity from Boost's placeholders
#define BOOST_BIND_NO_PLACEHOLDERS

#include <boost/config/warning_disable.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include "ast.hpp"

namespace rematch {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace unicode = boost::spirit::unicode;

template <typename It> // It : Iterator type
struct parser : qi::grammar<It, ast::altern()> {
  parser() : parser::base_type(altern_) {
    using qi::attr;
    using qi::eol;
    using qi::eps;
    using qi::hex;
    using qi::lexeme;
    using qi::lit;
    using qi::matches;
    using qi::omit;
    using qi::uint_;
    using unicode::alnum;
    using unicode::alpha;
    using unicode::char_;
    using unicode::no_case;

    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;

    using qi::debug;
    using qi::fail;
    using qi::on_error;

    using phoenix::construct;
    using phoenix::val;

    using namespace qi::labels;

    /////////////////////////
    ////  GRAMMAR RULES  ////
    /////////////////////////

    altern_ = concat_ % '|';

    concat_ = +(iter_);

    iter_ = group_ >> *rep_;

    rep_ = lit('?')[_val = ast::repetition(0, 1)] |
           lit('*')[_val = ast::repetition(0, -1)] |
           lit('+')[_val = ast::repetition(1, -1)] |
           ('{' >> uint_ >> '}')[_val = construct<ast::repetition>(_1, _1)] |
           ("{," >> uint_ >> '}')[_val = construct<ast::repetition>(0, _1)] |
           ('{' >> uint_ >> ",}")[_val = construct<ast::repetition>(_1, -1)] |
           ('{' >> uint_ >> ',' >> uint_ >>
            '}')[_val = construct<ast::repetition>(_1, _2)];

    group_ = parenthesis_ | assign_ | atom_;

    parenthesis_ = '(' >> altern_ >> ')';

    assign_ = '!' >> var_ >> '{' >> altern_ >> '}';

    atom_ = charset_ | special_ | symb_;

    assert_ = lit('^')[_val = ast::assertion(AssertionCode::kStartAnchor)] |
              lit('$')[_val = ast::assertion(AssertionCode::kEndAnchor)] |
              lit("\\b")[_val = ast::assertion(AssertionCode::kWordBoundary)] |
              lit("\\B")[_val = ast::assertion(AssertionCode::kNoWordBoundary)];

    special_ =
        lit(".")[_val = ast::special(SpecialCode::kAnyChar, true)] |
        lit("\\d")[_val = ast::special(SpecialCode::kAnyDigit, true)] |
        lit("\\D")[_val = ast::special(SpecialCode::kAnyDigit, false)] |
        lit("\\w")[_val = ast::special(SpecialCode::kAnyWord, true)] |
        lit("\\W")[_val = ast::special(SpecialCode::kAnyWord, false)] |
        lit("\\s")[_val = ast::special(SpecialCode::kAnyWhiteSpace, true)] |
        lit("\\S")[_val = ast::special(SpecialCode::kAnyWhiteSpace, false)];

    symb_ = (unesc_char_ | "\\" >> char_("\\+*?(){}[]|!.-") |
             ~char_("\\+*?(){}[]|.") // Anything but '+*?(){}[]|.'
    );

    // Inside a charclass is not necesary to escape some symbols (e.g. ".")
    charclass_symb_ = (unesc_char_ | charclass_unesc_char_ | ~char_("\\[]"));

    charset_ = '[' >> ('^' >> attr(true) | attr(false)) >>
               +(range_ | charclass_symb_) >> ']';

    range_ = charclass_symb_ >> '-' >> charclass_symb_;

    var_ = alpha >> *(alnum);

    unesc_char_.add("\\a", '\a')("\\f", '\f')("\\n", '\n')("\\r", '\r')(
        "\\t", '\t')("\\v", '\v')("\\\\", '\\')("\\\"", '"');

    charclass_unesc_char_.add("\\.", '.')("\\+", '+')("\\*", '*')("\\?", '?')(
        "\\)", ')')("\\)", ')')("\\}", '}')("\\{", '{')("\\|", '|')("\\^", '^')(
        "\\]", ']')("\\[", '[')("\\!", '!');

    // Rule naming (for debugging)

    altern_.name("alternation");
    concat_.name("concatenation");
    iter_.name("iteration");
    group_.name("group");
    parenthesis_.name("parenthesis");
    rep_.name("repetition");
    var_.name("variable");
    atom_.name("atom");
    assign_.name("assign");
    charset_.name("charset");
    charclass_symb_.name("cs_symbol");
    symb_.name("symbol");
    range_.name("range");

    /////////////////////
    ////  DEBUGGING  ////
    /////////////////////

    on_error<fail>(altern_,
                   std::cout
                       << phoenix::val("Error! Expecting ")
                       << boost::spirit::_4 // what failed?
                       << phoenix::val(" here: \"")
                       << construct<std::string>(
                              boost::spirit::_3,
                              boost::spirit::_2) // iterators to error-pos, end
                       << phoenix::val("\"") << std::endl);

    // debug(altern_);
    // debug(concat_);
    // debug(iter_);
    // debug(group_);
    // debug(parenthesis_);
    // debug(rep_);
    // debug(var_);
    // debug(atom_);
    // debug(assign_);
    // debug(charset_);
    // debug(charclass_symb_);
    // debug(symb_);
    // debug(range_);

  } // end parser (Constructor)

private:
  // Rule declaration

  qi::rule<It, ast::altern()> altern_;
  qi::rule<It, ast::concat()> concat_;
  qi::rule<It, ast::iter()> iter_;
  qi::rule<It, ast::group()> group_;
  qi::rule<It, ast::parenthesis()> parenthesis_;
  qi::rule<It, ast::repetition()> rep_;
  qi::rule<It, std::string()> var_;
  qi::rule<It, ast::atom()> atom_;
  qi::rule<It, ast::assignation()> assign_;
  qi::rule<It, ast::charset()> charset_;
  qi::rule<It, char()> symb_;
  qi::rule<It, char()> charclass_symb_;
  qi::rule<It, ast::assertion()> assert_;
  qi::rule<It, ast::charset::range()> range_;
  qi::rule<It, ast::special()> special_;

  qi::symbols<char const, char const> unesc_char_;
  qi::symbols<char const, char const> charclass_unesc_char_;
};

} // end namespace rematch

#endif