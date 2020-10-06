#ifndef AUTOMATA_MACRODFA
#define AUTOMATA_MACRODFA

#include <vector>
#include <functional>

#include "automata/macrodfa/macrostate.hpp"

class MacroDFA {
 public:
  MacroDFA() = default;

  MacroState& add_state(DetState *state);
  MacroState& add_state(std::vector<DetState*> states);

  void set_as_init(MacroState &ms);
  void set_as_final(MacroState &ms);

  MacroState& get_init_state();

 private:
  MacroState *init_state_; // Raw ptr
  std::vector<MacroState> states_;
  std::vector<std::reference_wrapper<MacroState>> final_states_;
}; // end class MacroDFA

#endif // AUTOMATA_MACRODFA