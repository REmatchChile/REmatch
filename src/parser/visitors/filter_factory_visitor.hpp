#ifndef PARSER__VISITORS__FILTER_FACTORY_VISITOR
#define PARSER__VISITORS__FILTER_FACTORY_VISITOR

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include "factories/factories.hpp"

namespace rematch {
namespace visitors {

using ff_ptr = std::unique_ptr<FilterFactory>;

struct regex2filters : boost::static_visitor<void> {
	private:
		ff_ptr filter_factory_;
	public:
		regex2filters();
		void operator()(ast::altern const &a);
		void operator()(ast::concat const &c);
		void operator()(ast::iter const &it);
		void operator()(ast::group const &g);
		void operator()(ast::parenthesis const &p);
		void operator()(ast::assignation const &a);
		void operator()(ast::atom const &a);
		void operator()(ast::charset const &cs);
		void operator()(char const &a);
		void operator()(ast::special const &a);
		void operator()(ast::assertion const &a);

		ff_ptr get_factory();
};

} // end namespace visitors
} // end namespace rematch

#endif // PARSER__VISITORS__FILTER_FACTORY_VISITOR