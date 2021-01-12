#ifndef FACTORIES__VARFACTORY_HPP
#define FACTORIES__VARFACTORY_HPP

#include <string>
#include <bitset>
#include <vector>

#include "factories/absfactory.hpp"

using CaptureBitmap = std::bitset<32>;

const uint8_t kMaxCaptureVars = 16;

class VariableFactory
  : public AbsFactory<std::string> {

  VariableFactory(): AbsFactory() {}

  // Gets the corresponding capture code (bitmap) as if the
  // capture variable were opened.
  CaptureBitmap get_open_capture_code(std::string var) const;

  // Gets the corresponding capture code (bitmap) as if the
  // capture variable were closed.
  CaptureBitmap get_close_capture_code(std::string var) const;

  // Outputs a string representation given a bitmap capture code.
  std::string capture_repr(CaptureBitmap bm) const;

  // Inplace merging with another VariableFactory
  void merge(VariableFactory const &rhs);

  friend std::ostream& operator<<(std::ostream &os, VariableFactory const &rhs);


}; // end class VariableFactory

#endif // FACTORIES__VARFACTORY_HPP