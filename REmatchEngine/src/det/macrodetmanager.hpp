#ifndef DET__MACRODETMANAGER
#define DET__MACRODETMANAGER

#include <unordered_map>
#include <memory>

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/detautomaton.hpp"
#include "bitsetwrapper.hpp"

class MacroDetManager {
 public:
  MacroDetManager();

  MacroTransition& next_transition(MacroState &s, char a);
 private:
  std::unordered_map<BitsetWrapper, MacroState*> mstates_table_;
  std::shared_ptr<DetAutomaton> dfa_;
  std::shared_ptr<MacroDFA> mdfa_;
}; // end class MacroDetManager

#endif // DET__MACRODETMANAGER