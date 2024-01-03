#ifndef LIBRARY_INTERFACE__FLAGS_HPP
#define LIBRARY_INTERFACE__FLAGS_HPP

#include <cstdarg>
#include <utility>
#include <stdexcept>

namespace REMatch {
inline namespace library_interface {

class Flags {
  /***************************************************************
    * Linebyline  (default=false)                                 *
    * El documento se carga línea por línea.                      *
    *                                                             *
    * Earlyoutput (default=false)                                 *
    * El algoritmo entrega outputs a medida que los encuentra, de *
    * otra forma el algoritmo escanea todo el documento y luego   *
    * enumera.                                                    *
    ***************************************************************/
public:
  enum FlagTypes {
    LineByLine,
    EarlyOutput
  };

  bool line_by_line = false;
  bool early_output = false;

  Flags() = default;
  Flags(int num, ...);
};

}
}

#endif
