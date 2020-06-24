#ifndef PYINTERFACE_HPP
#define PYINTERFACE_HPP

#include "regex/regex.hpp"

namespace rematch {

class PyInterface {

 public:
  PyInterface();

  // TODO: Check case with no capture.
  Match next();

  bool hasNext();

 private:
  Evaluation *evaluation_;

}; // end class PyInterface

} // end namespace rematch

#endif // PYINTERFACE_HPP