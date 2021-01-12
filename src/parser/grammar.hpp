// #define BOOST_SPIRIT_DEBUG
#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include "ast.hpp"

namespace qi  = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename It> // It : Iterator type
struct parser : qi::grammar<It, ast::altern()> // altern : type returned at parsing
{
    parser() : parser::base_type(altern_) // Starting rule
    /* Parser constructor */
    {
        //////////////////////////////
        ////  USING DECLARATIONS  ////
        //////////////////////////////

        using qi::lexeme;
        using qi::lit;
        using ascii::char_;
        using ascii::alpha;
        using ascii::alnum;
        using ascii::no_case;
        using qi::attr;
        using qi::uint_;
        using qi::hex;
        using qi::eps;
        using qi::eol;
        using qi::omit;
        using qi::matches;

        using qi::on_error;
        using qi::fail;
        using qi::debug;

        using phoenix::construct;

        using namespace qi::labels;

        /////////////////////////
        ////  GRAMMAR RULES  ////
        /////////////////////////

        altern_ = concat_ % '|';

        concat_ = +(iter_);

        iter_ = group_ >> rep_;

        rep_ = *(char_('?') | char_('*') | char_('+'));

        group_ =  (parenthesis_) | (assign_) | (atom_) ;

        parenthesis_ =  '(' >> altern_ >> ')';

        assign_ =   '!' >> var_  >> '{' >> altern_ >> '}';

        atom_ =  (
                  charset_
                | "\\d" >> attr(ast::anydigit())
                | "\\D" >> attr(ast::nondigit())
                | "\\w" >> attr(ast::anyword())
                | "\\W" >> attr(ast::nonword())
                | "\\s" >> attr(ast::anywhitespace())
                | "." >> attr(ast::anychar())
                | symb_
                ) ; // Here we construct the atomic automaton

        // TODO: Revisar
        symb_ = (unesc_char |
                "\\" > char_ |
                ~char_("\\+*?(){}[]|.-") // Anything but '+*?(){}[]|.'
                );

        charset_ = '['
                  >> ('^' >> attr(true) | attr(false))
                  >> *(range_ | symb_) >
                  ']';

        range_ = symb_ >> '-' >> symb_;

        var_ = alpha >> *(alnum);

        unesc_char.add("\\a", '\a')("\\b", '\b')("\\f", '\f')("\\n", '\n')
                  ("\\r", '\r')("\\t", '\t')("\\v", '\v')("\\\\", '\\')
                  ("\\\"", '"');

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
        symb_.name("symbol");
        range_.name("range");

        /////////////////////
        ////  DEBUGGING  ////
        /////////////////////


        /*on_error<fail>
            (
                altern_
              , std::cout
                    << phoenix::val("Error! Expecting ")
                    << _4                               // what failed?
                    << phoenix::val(" here: \"")
                    << construct<std::string>(_3, _2)   // iterators to error-pos, end
                    << phoenix::val("\"")
                    << std::endl
            );

        debug(altern_);
        debug(concat_);
        debug(iter_);
        debug(group_);
        debug(parenthesis_);
        debug(rep_);
        debug(var_);
        debug(atom_);
        debug(assign_);*/

    } // Constructor

  private:
    // Rule declaration

    qi::rule<It, ast::altern()> altern_;
    qi::rule<It, ast::concat()> concat_;
    qi::rule<It, ast::iter()> iter_;
    qi::rule<It, ast::group()> group_;
    qi::rule<It, ast::parenthesis()> parenthesis_;
    qi::rule<It, std::vector<char>()> rep_;
    qi::rule<It, std::string()> var_;
    qi::rule<It, ast::atom()> atom_;
    qi::rule<It, ast::assignation()> assign_;
    qi::rule<It, ast::charset()> charset_;
    qi::rule<It, char()> symb_;
    qi::rule<It, ast::charset::range()> range_;
    qi::symbols<char const, char const> unesc_char;

};


#endif