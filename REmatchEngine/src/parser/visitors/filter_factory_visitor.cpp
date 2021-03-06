#include "filter_factory_visitor.hpp"

#include "factories/factories.hpp"

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
	filter_factory_->addFilter(CharClass(cs));
}

void regex2filters::operator()(char const &a) {
	filter_factory_->addFilter(CharClass(a));
}

void regex2filters::operator()(ast::anychar const &a) {
	filter_factory_->addFilter(CharClass(ANYCHAR, false));
}

void regex2filters::operator()(ast::anydigit const &a) {
	filter_factory_->addFilter(CharClass(ANYDIGIT, false));
}

void regex2filters::operator()(ast::nondigit const &a) {
	filter_factory_->addFilter(CharClass(ANYDIGIT, true));
}

void regex2filters::operator()(ast::anyword const &a) {
	filter_factory_->addFilter(CharClass(ANYWORD, false));
}

void regex2filters::operator()(ast::nonword const &a)  {
	filter_factory_->addFilter(CharClass(ANYWORD, true));
}

void regex2filters::operator()(ast::anywhitespace const &a)  {
	filter_factory_->addFilter(CharClass(ANYSPACE, false));
}

} // end namespace visitors