#include "automata_visitor.hpp"

#include "parse/regex/ast.hpp"
#include "automata/nfa/lva.hpp"
#include "automata/nfa/state.hpp"
#include "factories/factories.hpp"
#include "charclass.hpp"

namespace rematch {
namespace visitors {

regex2LVA::regex2LVA(std::shared_ptr<VariableFactory>& v,
										 std::shared_ptr<FilterFactory>& f)
		: v_fact_(v), f_fact_(f) {}

lva_ptr regex2LVA::operator()(ast::altern const &a) const {
	auto A = (*this)(a.front());
	if(a.size()>1) {
		for (size_t i = 1; i < a.size(); ++i) {
			A->alter(*(*this)(a[i]));
		}
	}
	return A;
}

lva_ptr regex2LVA::operator()(ast::concat const &c) const {
	auto A = (*this)(c.front());
	if(c.size()>1) {
		for (size_t i = 1; i < c.size(); ++i) {
			A->cat(*(*this)(c[i]));
		}
	}
	return A;
}

lva_ptr regex2LVA::operator()(ast::iter const &it) const {
	auto A = (*this)(it.expr);
	for (auto &rep: it.repetitions) {
		assert(rep.start_ >= 0);
		if(rep.start_ == 0 && rep.end_ == -1)
			A->kleene();
		else if(rep.start_ == 0 && rep.end_ == 1)
			A->optional();
		else if(rep.start_ == 1 && rep.end_ == -1)
			A->strict_kleene();
		else
			A->repeat(rep.start_, rep.end_);
	}

	return A;
}

lva_ptr regex2LVA::operator()(ast::group const &g) const {
	return boost::apply_visitor(*this, g);
}

lva_ptr regex2LVA::operator()(ast::parenthesis const &p) const {
	return (*this)(p.root);
}

lva_ptr regex2LVA::operator()(ast::assignation const &a) const {
	auto A = (*this)(a.root);
	std::bitset<32> op_code = v_fact_->open_code(a.var);
	std::bitset<32> cl_code = v_fact_->close_code(a.var);
	A->assign(op_code, cl_code);
	return A;
}

lva_ptr regex2LVA::operator()(ast::atom const &a) const {
	return boost::apply_visitor(*this, a);
}

lva_ptr regex2LVA::operator()(ast::charset const &cs) const {
	int code = f_fact_->getCode(CharClass(cs));
	return std::make_unique<LogicalVA>(code);
}

lva_ptr regex2LVA::operator()(char const &a) const {
	int code = f_fact_->getCode(a);
	return std::make_unique<LogicalVA>(code);
}

lva_ptr regex2LVA::operator()(ast::special const &a) const {
	int code = f_fact_->getCode(CharClass(a));
	return std::make_unique<LogicalVA>(code);
}

lva_ptr regex2LVA::operator()(ast::assertion const &a) const {
	// TODO
}


} // end namespace visitors
} // end namespace rematch