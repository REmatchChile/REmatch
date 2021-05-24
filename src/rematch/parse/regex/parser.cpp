#include <string>
#include <cassert>

#include "parser.hpp"
#include "visitors.hpp"
#include "grammar.hpp"
#include "parse/regex/exceptions.hpp"

namespace rematch {

bool doParse(const std::string& input, ast::altern &data)
/* Parses input and stores the AST in data */
{

    static const parser<std::string::const_iterator> p; // Parser

    auto f(begin(input)), l(end(input));
    bool ok = qi::parse(f,l,p, data);
    if (!ok)
      std::cerr << "parse failed: '" << std::string(f,l) << "'\n";

    if (f!=l) {
      std::stringstream ss;
      ss << "trailing unparsed: '" << std::string(f,l) << "'\n";
      throw parsing::BadRegex(ss.str());
    }
    return ok;
}


std::unique_ptr<LogicalVA> regex2LVA(std::string regex) {
  ast::altern tree;

 	doParse(regex, tree);

	std::shared_ptr<VariableFactory> v = visitors::regex2vars()(tree);

  auto v1 = visitors::regex2filters();
  v1(tree);
	std::shared_ptr<FilterFactory> f = v1.get_factory();

  auto A = visitors::regex2LVA(v,f)(tree);
  A->set_factories(v, f);

	return A;
}

} // end namespace rematch