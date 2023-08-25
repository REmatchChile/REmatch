
#include <vector>
#include <set>
#include "../nfa/extended_va.hpp"

namespace rematch {  
  using StatesBitset = std::vector<bool>;
  using StatesPtrSet = std::set<ExtendedVAState*>;
}
