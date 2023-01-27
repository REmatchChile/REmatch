#ifndef LIBRARY_INTERFACE__FLAGS_HPP
#define LIBRARY_INTERFACE__FLAGS_HPP

namespace REMatch {
inline namespace library_interface {

class Flags {
  /***************************************************************
    * Multiline(default=false)                                    *
    * Los símbolos ^ y $ hacen match con \n                      *
    *                                                             *
    * Dotnewline (default=false)                                  *
    * El símbolo . hace match con \n                             *
    *                                                             *
    * Linebyline  (default=false)                                 *
    * El documento se carga línea por línea.                    *
    *                                                             *
    * Earlyoutput (default=false)                                 *
    * El algoritmo entrega outputs a medida que los encuentra, de *
    * otra forma el algoritmo escanea todo el documento y luego   *
    * enumera.                                                    *
    *                                                             *
    * SaveAnchors (default=false)                                 *
    * Guarda el group[0] (match completo del regex)               *
    ***************************************************************/
public:
  enum FlagTypes {
    MultiLine,
    DotNewLine,
    LineByLine,
    EarlyOutput,
    SaveAnchors
  };

  bool multi_line = false;
  bool dot_new_line = false;
  bool line_by_line = false;
  bool early_output = false;
  bool save_anchors = false;

  Flags() = default;
  Flags(int num, ...);
};

}
}

#endif
