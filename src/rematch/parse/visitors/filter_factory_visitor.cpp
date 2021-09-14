#include "filter_factory_visitor.hpp"

#include "factories/factories.hpp"

namespace rematch {
namespace visitors {

regex2filters::regex2filters() {
	filter_factory_ = std::make_unique<FilterFactory>();
}

ff_ptr regex2filters::get_factory() {
	return std::move(filter_factory_);
}

void regex2filters::operator()(ast::altern const &a) {
	for(auto &elem: a)
		(*this)(elem);
}

void regex2filters::operator()(ast::concat const &c) {
	for(auto &elem: c) {
		(*this)(elem);
	}
}

void regex2filters::operator()(ast::iter const &it) {
	(*this)(it.expr);
}

void regex2filters::operator()(ast::group const &g) {
	boost::apply_visitor(*this, g);
}

void regex2filters::operator()(ast::parenthesis const &p) {
	(*this)(p.root);
}

void regex2filters::operator()(ast::assignation const &a) {
	(*this)(a.root);
}

void regex2filters::operator()(ast::atom const &a) {
	boost::apply_visitor(*this, a);
}

void regex2filters::operator()(ast::charset const &cs) {
	filter_factory_->add_filter(cs); // Implicit conversion
}

void regex2filters::operator()(char const &a) {
	filter_factory_->add_filter(a); // Implicit conversion
}

void regex2filters::operator()(ast::special const &s) {
	filter_factory_->add_filter(s); // Implicit conversion
}

void regex2filters::operator()(ast::assertion const &a) {
	// TODO
}

} // end namespace visitors
} // end namespace rematch