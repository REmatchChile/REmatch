#ifndef VISITORS_HPP
#define VISITORS_HPP

#include <iostream>
#include <sstream>
#include <cassert>
#include <exception>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include "ast.hpp"
#include "automata/lvastate.hpp"
#include "automata/lva.hpp"
#include "factories.hpp"

using boost::get;

namespace visitors {

struct bad_regex: public std::exception {
	const char * what() const throw() {
		return "Not a functional regex.";
	}
};

struct regex2LVA : boost::static_visitor<LogicalVA&> {
	private:
		VariableFactory& vFact;
		FilterFactory& fFact;
	public:

		regex2LVA(VariableFactory& v, FilterFactory& f);

		LogicalVA &operator()(ast::altern const &a) const;
		LogicalVA &operator()(ast::concat const &c) const;
		LogicalVA &operator()(ast::iter const &it) const;
		LogicalVA &operator()(ast::group const &g) const;
		LogicalVA &operator()(ast::parenthesis const &p) const;
		LogicalVA &operator()(ast::assignation const &a) const;
		LogicalVA &operator()(ast::atom const &a) const;
		LogicalVA &operator()(ast::charset const &cs) const;
		LogicalVA &operator()(char const &a) const;
		LogicalVA &operator()(ast::anychar const &a) const;
		LogicalVA &operator()(ast::anydigit const &a) const;
		LogicalVA &operator()(ast::nondigit const &a) const;
		LogicalVA &operator()(ast::anyword const &a) const;
		LogicalVA &operator()(ast::nonword const &a) const;
		LogicalVA &operator()(ast::anywhitespace const &a) const;
};

struct regex2vars : boost::static_visitor<VariableFactory&> {
		// TODO: "Singleton" variable factory

		VariableFactory &operator()(ast::altern const &a) const;
		VariableFactory &operator()(ast::concat const &c) const;
		VariableFactory &operator()(ast::iter const &it) const;
		VariableFactory &operator()(ast::group const &g) const;
		VariableFactory &operator()(ast::parenthesis const &p) const;
		VariableFactory &operator()(ast::assignation const &a) const;
		VariableFactory &operator()(ast::atom const &a) const;
};

struct regex2filters : boost::static_visitor<FilterFactory&> {
	private:
		FilterFactory *m_filterFactory;
	public:
		regex2filters();

		FilterFactory &operator()(ast::altern const &a);
		FilterFactory &operator()(ast::concat const &c);
		FilterFactory &operator()(ast::iter const &it);
		FilterFactory &operator()(ast::group const &g);
		FilterFactory &operator()(ast::parenthesis const &p);
		FilterFactory &operator()(ast::assignation const &a);
		FilterFactory &operator()(ast::atom const &a);
		FilterFactory &operator()(ast::charset const &cs);
		FilterFactory &operator()(char const &a);
		FilterFactory &operator()(ast::anychar const &a);
		FilterFactory &operator()(ast::anydigit const &a);
		FilterFactory &operator()(ast::nondigit const &a);
		FilterFactory &operator()(ast::anyword const &a);
		FilterFactory &operator()(ast::nonword const &a);
		FilterFactory &operator()(ast::anywhitespace const &a);
};

} // Namespace visitors

#endif