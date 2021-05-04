#ifndef AUTOMATA_MACRODFA
#define AUTOMATA_MACRODFA

#include <vector>
#include <functional>
#include <memory>

#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

class MacroDFA {
 public:
  MacroDFA() = default;

  MacroState* add_state(DetState *state);
  MacroState* add_state(std::vector<DetState*> states);

  void set_as_init(MacroState *ms);

  MacroState& get_init_state();

  size_t size() {return states_.size();}

 private:
  MacroState *init_state_; // Raw ptr
  std::vector<std::shared_ptr<MacroState>> states_;
  // std::vector<std::shared_ptr<MacroState>> final_states_;
}; // end class MacroDFA

} // end namespace rematch

#endif // AUTOMATA_MACRODFA