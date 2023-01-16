#ifndef LIBRARY_INTERFACE__FLAGS_HPP
#define LIBRARY_INTERFACE__FLAGS_HPP

namespace rematch {

inline namespace library_interface {
  class Flags {
  public:
    // TODO: following flags:
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
    // options for instantiation:
    // 1. enums (e.g. Flags(Flags::Multiline, Flags::Dotnewline)),
    //    order would not matter as it could be passed as an elipsis or
    //    variadic-templates (preffered).
    // 2. individual functions :
    //    flags = Flags(); flags.setMultiline(); flags.setDotnewline();
    // 3. constructor (many parameters)
    //    Flags flags(true, false, true true false);
      
    };
  };
} // end namespace library_interface

} // end namespace rematch

#endif
