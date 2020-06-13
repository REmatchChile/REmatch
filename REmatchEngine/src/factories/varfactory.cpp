#include "factories/varfactory.hpp"

#include <sstream>

CaptureBitmap VariableFactory::get_open_capture_code(std::string var) const {
  auto code = get_code(var);
  CaptureBitmap bitmap;

  bitmap.set(code*2);

  return bitmap;
}

CaptureBitmap VariableFactory::get_close_capture_code(std::string var) const {
  auto code = get_code(var);
  CaptureBitmap bitmap;

  bitmap.set(code*2 + 1);

  return bitmap;
}

std::string VariableFactory::capture_repr(CaptureBitmap bm) const {
  std::stringstream ss;

  for(size_t i=0; i < (int)(bm.size()/2); i++) {
    if(bm[2*i])     ss << get_element(i) << "<";
    if(bm[2*i + 1]) ss << get_element(i) << ">";
  }

  return ss.str();
}

void VariableFactory::merge(VariableFactory const &rhs) {
  
}