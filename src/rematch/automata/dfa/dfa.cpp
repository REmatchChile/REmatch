#include "dfa.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>
#include <cassert>
#include <map>

#include "structs/dag/nodelist.hpp"
#include "captures.hpp"
#include "factories/factories.hpp"
#include "detstate.hpp"
#include "automata/nfa/eva.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

DFA::DFA(VariableFactory* vf)
  : init_state_(new DetState()), variable_factory_(vf)  {states.push_back(init_state_);}

DFA::DFA(ExtendedVA &a)
 : init_state_(new DetState()), variable_factory_(a.varFactory()) {
   states.push_back(init_state_);
}

std::string DFA::pprint() {
  /* Gives a codification for the automaton that can be used to visualize it
     at https://puc-iic2223.github.io . It uses Breath-First Search
     to get every labeled transition in the automaton with the unique ids for
     each state */

  // Declarations
  std::stringstream ss, cond;
  DetState *current;
  std::bitset<32> S;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<DetState*> queue;

  // Start on the init state
  visited.insert(init_state_->id);
  queue.push_back(init_state_);

  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();

    for (size_t i = 0; i < 128; i++) {
      if(current->transitions_[i] == nullptr) continue;
      auto trans = current->transitions_[i].get();
      cond.clear();
      cond << "t " << *current << ' ' ;
      if(i == 0)
          ss << "0x0";
        else if((char)i == '\n')
          ss << "\\n";
        else if((char)i == ' ')
          ss << "' '";
        else if((char)i == '\t')
          ss << "\\t";
        else
          ss << (char)i;
      std::string prefix = cond.str();
      if(trans->type_ & Transition::Type::kDirect) {// Direct type
        ss << prefix << ' ' << trans->direct_->id;
        if (visited.find(trans->direct_->id) == visited.end()) {
          visited.insert(trans->direct_->id);
          queue.push_back(trans->direct_);
        }
      }
      if(trans->type_ & Transition::Type::kSingleCapture) {
        ss << prefix << '/' << variable_factory_->print_varset(trans->capture_->S)
                     << ' ' << trans->capture_->next->id;
        if (visited.find(trans->capture_->next->id) == visited.end()) {
          visited.insert(trans->capture_->next->id);
          queue.push_back(trans->capture_->next);
        }
      } // Single capture
      if(trans->type_ & Transition::Type::kMultiCapture) {
        for(auto &capture: trans->captures_) {
          ss << prefix << '/' << variable_factory_->print_varset(capture->S)
                       << ' ' << capture->next->id;
          if (visited.find(capture->next->id) == visited.end()) {
            visited.insert(capture->next->id);
            queue.push_back(capture->next);
          }
        }
      }
    }
  }

  // Code final states
  for (size_t i = 0; i < finalStates.size(); ++i) {
    if(finalStates[i]->isFinal) {
      ss << "f " << *finalStates[i] << '\n';
    }
  }

  // Code initial state
  ss << "i " << *init_state_;

  return ss.str();
}

} // end namespace rematch
