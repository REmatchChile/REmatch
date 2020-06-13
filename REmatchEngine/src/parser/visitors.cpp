#include <iostream>
#include <sstream>
#include <cassert>
#include <exception>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include "visitors.hpp"
#include "ast.hpp"
#include "automata/lvastate.hpp"
#include "automata/lva.hpp"
#include "factories/factories.hpp"

using boost::get;

namespace visitors {

regex2LVA :: regex2LVA(VariableFactory& v, FilterFactory& f): vFact(v), fFact(f) {}

LogicalVA& regex2LVA :: operator()(ast::altern const &a) const {
	LogicalVA& A = (*this)(a.front());
	if(a.size()>1) {
		for (size_t i = 1; i < a.size(); ++i) {
			A.alter((*this)(a[i]));
		}
	}
	return A;
}

LogicalVA& regex2LVA :: operator()(ast::concat const &c) const {
	LogicalVA& A = (*this)(c.front());
	if(c.size()>1) {
		for (size_t i = 1; i < c.size(); ++i) {
			A.cat((*this)(c[i]));
		}
	}
	return A;
}

LogicalVA& regex2LVA :: operator()(ast::iter const &it) const {
	LogicalVA& A = (*this)(it.expr);
	for (size_t i = 0; i < it.repetitions.size(); ++i) {
		switch(it.repetitions[i]) {
			case '?':
				A.optional(); break;
			case '*':
				A.kleene(); break;
			case '+':
				A.strict_kleene(); break;
		}
	}

	return A;
}

LogicalVA& regex2LVA :: operator()(ast::group const &g) const {
	return boost::apply_visitor(*this, g);
}

LogicalVA& regex2LVA :: operator()(ast::parenthesis const &p) const {
	return (*this)(p.root);
}

LogicalVA& regex2LVA :: operator()(ast::assignation const &a) const {
	LogicalVA& A = (*this)(a.root);
	A.assign(a.var);
	return A;
}

LogicalVA& regex2LVA :: operator()(ast::atom const &a) const {
	return boost::apply_visitor(*this, a);
}

LogicalVA& regex2LVA :: operator()(ast::charset const &cs) const {
	LogicalVA* A = new LogicalVA(vFact, fFact);
	LVAState* fState = new LVAState();
	fState->setFinal(true);
	A->states.push_back(fState);
	A->finalStates.push_back(fState);

	CharClass chs = CharClass(cs);

	A->initState->addFilter(fFact.getCode(chs), fState);

	return *A;
}

LogicalVA& regex2LVA :: operator()(char const &a) const {
	LogicalVA* A = new LogicalVA(a, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::anychar const &a) const {
	LogicalVA* A = new LogicalVA(ANYCHAR, false, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::anydigit const &a) const {
	LogicalVA* A = new LogicalVA(ANYDIGIT, false, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::nondigit const &a) const {
	LogicalVA* A = new LogicalVA(ANYDIGIT, true, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::anyword const &a) const {
	LogicalVA* A = new LogicalVA(ANYWORD, false, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::nonword const &a) const {
	LogicalVA* A = new LogicalVA(ANYWORD, true, vFact, fFact);
	return *A;
}

LogicalVA& regex2LVA :: operator()(ast::anywhitespace const &a) const {
	LogicalVA* A = new LogicalVA(ANYSPACE, false, vFact, fFact);
	return *A;
}


		// TODO: "Singleton" variable factory

VariableFactory& regex2vars :: operator()(ast::altern const &a) const {
	VariableFactory& V = (*this)(a.front());
	if(a.size()>1) {
		for (size_t i = 1; i < a.size(); ++i) 
			if(!(V == (*this)(a[i]))) {
				throw bad_regex();
			}; // Check for functional regex
	}
	return V;
}

VariableFactory& regex2vars :: operator()(ast::concat const &c) const {
	VariableFactory& V = (*this)(c.front());
	if(c.size()>1) {
		for (size_t i = 1; i < c.size(); ++i) {
			V.merge((*this)(c[i])); // Checking for functional regex inside merge
		}
	}
	return V;
}

VariableFactory& regex2vars :: operator()(ast::iter const &it) const {
	VariableFactory& V = (*this)(it.expr);
	if(!it.repetitions.empty()){
		assert(V.isEmpty()); // Check for functional regex
	}
	return V;
}

VariableFactory& regex2vars :: operator()(ast::group const &g) const {
	return boost::apply_visitor(*this, g);
}

VariableFactory&  regex2vars :: operator()(ast::parenthesis const &p) const {
	return (*this)(p.root);
}

VariableFactory&  regex2vars :: operator()(ast::assignation const &a) const {
	VariableFactory& V = (*this)(a.root);
	assert(!V.isMember(a.var)); // Check for functional regex
	V.addVar(a.var);
	return V;
}

VariableFactory&  regex2vars :: operator()(ast::atom const &a) const {
	return *(new VariableFactory());
}


regex2filters :: regex2filters() {
	m_filterFactory = new FilterFactory();
}

FilterFactory& regex2filters :: operator()(ast::altern const &a) {
	for(auto &elem: a) {
		(*this)(elem);
	}
	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::concat const &c) {
	for(auto &elem: c) {
		(*this)(elem);
	}

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::iter const &it) {
	(*this)(it.expr);

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::group const &g) {
	boost::apply_visitor(*this, g);

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::parenthesis const &p) {
	(*this)(p.root);

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::assignation const &a) {
	(*this)(a.root);

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::atom const &a) {
	boost::apply_visitor(*this, a);

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::charset const &cs) {
	m_filterFactory->addFilter(CharClass(cs));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(char const &a) {
	m_filterFactory->addFilter(CharClass(a));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::anychar const &a) {
	m_filterFactory->addFilter(CharClass(ANYCHAR, false));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::anydigit const &a) {
	m_filterFactory->addFilter(CharClass(ANYDIGIT, false));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::nondigit const &a) {
	m_filterFactory->addFilter(CharClass(ANYDIGIT, true));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::anyword const &a) {
	m_filterFactory->addFilter(CharClass(ANYWORD, false));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::nonword const &a)  {
	m_filterFactory->addFilter(CharClass(ANYWORD, true));

	return *m_filterFactory;
}

FilterFactory& regex2filters :: operator()(ast::anywhitespace const &a)  {
	m_filterFactory->addFilter(CharClass(ANYSPACE, false));

	return *m_filterFactory;
}

} // Namespace visitors

