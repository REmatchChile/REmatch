#include "detautomaton.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>
#include <cassert>
#include <map>

#include "structures.hpp"
#include "captures.hpp"
#include "factories/factories.hpp"
#include "detstate.hpp"
#include "automata/eva.hpp"
#include "automata/transition.hpp"

DetAutomaton :: DetAutomaton(VariableFactory* vf)
  : init_state_(new DetState()), variable_factory_(vf)  {states.push_back(init_state_);}

DetAutomaton::DetAutomaton(ExtendedVA &a)
 : init_state_(new DetState()), variable_factory_(a.varFactory()) {
   states.push_back(init_state_);
}

std::string DetAutomaton :: pprint() {
  /* Gives a codification for the automaton that can be used to visualize it
     at https://puc-iic2223.github.io . It uses Breath-First Search
     to get every labeled transition in the automaton with the unique ids for
     each state */

  // Declarations
  std::stringstream ss, cond;
  DetState *current;
  int nid;  // cid: current state id; nid : next state id
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
      for(auto &capture: current->transitions_[i]->captures()) {
        DetState* it = capture->next;
        if(it == nullptr)
          continue;
        nid = it->id;
        ss << "t " << *current << ' ' ;

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

        if(capture->S.any())
          ss << '/' << variable_factory_->getVarUtil(capture->S);

        ss  << ' ' << *it << '\n';

        // If not visited enqueue and add to visited
        if (visited.find(nid) == visited.end()) {
          visited.insert(nid);
          queue.push_back(it);
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
