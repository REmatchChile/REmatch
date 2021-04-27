#ifndef PARSER__VISITORS__AUTOMATA_VISITOR
#define PARSER__VISITORS__AUTOMATA_VISITOR

#include <memory>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include "automata/nfa/lva.hpp"
#include "parse/regex/ast.hpp"

namespace rematch {
namespace visitors {

using lva_ptr = std::unique_ptr<LogicalVA>;

struct regex2LVA : boost::static_visitor<lva_ptr> {
	private:
		std::shared_ptr<VariableFactory> v_fact_;
		std::shared_ptr<FilterFactory> f_fact_;
	public:

		regex2LVA(std::shared_ptr<VariableFactory> &v, std::shared_ptr<FilterFactory> &f);

		lva_ptr operator()(ast::concat const &c) const;
		lva_ptr operator()(ast::iter const &it) const;
		lva_ptr operator()(ast::group const &g) const;
		lva_ptr operator()(ast::parenthesis const &p) const;
		lva_ptr operator()(ast::assignation const &a) const;
		lva_ptr operator()(ast::atom const &a) const;
		lva_ptr operator()(ast::altern const &a) const;
		lva_ptr operator()(ast::charset const &cs) const;
		lva_ptr operator()(char const &a) const;
		lva_ptr operator()(ast::special const &a) const;
		lva_ptr operator()(ast::assertion const &a) const;
};

} // end namespace visitors
} // end namespace rematch

#endif // PARSER__VISITORS__AUTOMATA_VISITOR