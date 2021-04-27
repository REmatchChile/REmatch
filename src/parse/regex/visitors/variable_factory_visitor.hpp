#ifndef PARSER__VISITORS__VARIABLE_FACTORY_VISITOR
#define PARSER__VISITORS__VARIABLE_FACTORY_VISITOR

#include <memory>

#include "parse/regex/ast.hpp"
#include "factories/factories.hpp"

namespace rematch {
namespace visitors {

using vfptr = std::unique_ptr<VariableFactory>;

struct regex2vars : boost::static_visitor<vfptr> {

	vfptr operator()(ast::altern const &a) const;
	vfptr operator()(ast::concat const &c) const;
	vfptr operator()(ast::iter const &it) const;
	vfptr operator()(ast::group const &g) const;
	vfptr operator()(ast::parenthesis const &p) const;
	vfptr operator()(ast::assignation const &a) const;
	vfptr operator()(ast::atom const &a) const;
};

} // end namespace visitors
} // end namespace rematch

#endif // PARSER__VISITORS__VARIABLE_FACTORY_VISITOR