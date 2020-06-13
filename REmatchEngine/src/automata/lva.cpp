#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>
#include <cassert>
#include <map>

#include "lva.hpp"
#include "structures.hpp"
#include "lvastate.hpp"
#include "factories/factories.hpp"


LogicalVA :: LogicalVA() {
  vFact = new VariableFactory();
  fFact = new FilterFactory();
  initState = new LVAState();
  initState->setInitial(true);
  states.push_back(initState);

  numStates = 1;
  numFilters = 0;
}

LogicalVA :: LogicalVA(VariableFactory &vFact, FilterFactory &fFact) {

  this->vFact = &vFact;
  this->fFact = &fFact;

  initState = new LVAState();
  states.push_back(initState);

  numStates = 1;
  numFilters = 0;

}

LogicalVA :: LogicalVA(const char &a) {
  initState = new LVAState();
  states.push_back(initState);

  LVAState* fState = new LVAState();
  finalStates.push_back(fState);
  states.push_back(fState);
  fState->setFinal(true);

  int coding = 1;

   // Connect initState with fState
  initState->addFilter(coding, fState);

  // Set numStates and numFilters
  numStates = 2;
  numFilters = 1;
}

LogicalVA :: LogicalVA(const char &a, VariableFactory &vFact, FilterFactory &fFact) {

  this->vFact = &vFact;
  this->fFact = &fFact;

  initState = new LVAState();
  states.push_back(initState);

  LVAState* fState = new LVAState();
  finalStates.push_back(fState);
  states.push_back(fState);
  fState->setFinal(true);

  int coding = this->fFact->getCode(a);

  // Connect initState with fState
  initState->addFilter(coding, fState);

  // Set numStates and numFilters
  numStates = 2;
  numFilters = 1;
}

LogicalVA :: LogicalVA(int spec, bool negated, VariableFactory &vFact, FilterFactory &fFact) {

  this->vFact = &vFact;
  this->fFact = &fFact;

  initState = new LVAState();
  states.push_back(initState);

  LVAState* fState = new LVAState();
  finalStates.push_back(fState);
  states.push_back(fState);
  fState->setFinal(true);

  int coding = this->fFact->getCode(CharClass(spec, negated));

  // Connect initState with fState
  initState->addFilter(coding, fState);

  // Set numStates and numFilters
  numStates = 2;
  numFilters = 1;
}

// LogicalVA::LogicalVA(const LogicalVA &A)
//   : numStates(A.numStates),
//     numFilters(A.numFilters),
//     numCaptures(A.numCaptures),
//     numEpsilons(A.numEpsilons),
//     vFact(A.vFact),
//     fFact(A.fFact)  {

//   // Mantain a map from the original states to their copies
// 	std::map<LVAState*, LVAState*> copy_table;
// 	LVAState *new_state;

// 	// Populate the copy table
// 	for(const auto &state: A.states) {
// 		new_state = new LVAState(*state); // Copy the state in heap
// 		states.push_back(new_state);
// 		if(new_state->isFinal)
// 			finalStates.push_back(new_state);
// 		copy_table[state] = new_state;
// 	}

// 	// Update init state
// 	initState = copy_table[A.initState];

// 	// Update transition table
// 	for(auto &state: this->states) {
// 		for(auto& capture: state->c)
// 			capture.reset_states(copy_table[capture.from], copy_table[capture.next]);
// 		for(auto& epsilon: state->e)
// 			epsilon.reset_states(copy_table[epsilon.next]);
// 		for(auto &filter: state->f)
// 			filter.reset_states(copy_table[filter.next]);
// 	}
// }

void LogicalVA::adapt_capture_jumping() {
  std::vector<LVAState*> stack;
  LVAState *reached_state;


  for(auto &state: states) {
    stack.clear();

    for(auto &capture: state->c) {
      stack.push_back(capture->next);
      state->tempMark = false;
    }

    while(!stack.empty()) {
      reached_state = stack.back();
      stack.pop_back();

      reached_state->tempMark = true;

      if(!reached_state->f.empty() || !reached_state->e.empty() || reached_state->isFinal)
        state->addEpsilon(reached_state);
      for(auto &capture: reached_state->c) {
        if(!reached_state->tempMark)
          stack.push_back(capture->next);
      }
    }
  }

  for(auto &state: states)
    state->c.clear();
}

void LogicalVA :: cat(LogicalVA &a2) {
  /* Concatenates an LogicalVA a2 to the current LogicalVA (inplace) */

  // TODO: Check if var sets set of both automata is disjunct

  // Adds eps transitions from final states to a2 init LVAState
  for(std::size_t i=0;i<finalStates.size();i++){
    finalStates[i]->addEpsilon(a2.initState);
    finalStates[i]->setFinal(false);
  }

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  // Set a2 final states as new final states
  finalStates.clear();
  finalStates = a2.finalStates;

  // Update numStates and numFilters
  numStates = states.size();
  numFilters += a2.numFilters;
}

void LogicalVA :: alter(LogicalVA &a2) {
  /* Extends the LogicalVA so it can alternate between itself and an
     LogicalVA a2 */

  // TODO: Check if var sets of both automata is the same

  // Creates a new init LVAState and connects it to the old init and a2's init
  LVAState* newinitState = new LVAState();
  newinitState->addEpsilon(initState);
  newinitState->addEpsilon(a2.initState);

  initState = newinitState;

  states.push_back(initState);
  numStates++;

  // Add a2 final states to final states list
  finalStates.insert(finalStates.end(),
    a2.finalStates.begin(), a2.finalStates.end());

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  // Update numStates and numFilters
  numStates = states.size();
  numFilters += a2.numFilters;
}

void LogicalVA :: kleene() {
  /* Extends the LogicalVA for kleene closure (0 or more times) */

  // // Create new init LVAState
  // LVAState* newinitState = new LVAState();
  // newinitState->addEpsilon(initState);
  // initState = newinitState;

  // // Add new LVAState to states list
  // states.push_back(initState);
  // numStates++;

  // Connect final states to new init LVAState
  for(std::size_t i=0;i<finalStates.size();i++){
    finalStates[i]->addEpsilon(initState);
    finalStates[i]->setFinal(false);
  }

  // Set new init as the final LVAState
  finalStates.clear();
  finalStates.push_back(initState);
  initState->setFinal(true);
}

void LogicalVA :: strict_kleene() {
  /* Extends the LogicalVA for strict kleene closure (1 or more times) */

  // Connect final states to init LVAState
  for(std::size_t i=0;i<finalStates.size();i++){
    finalStates[i]->addEpsilon(initState);
  }
}

void LogicalVA :: optional() {
  /* Extends the LogicalVA for optional closure (0 or 1 time) */

  // Set new init as a final LVAState
  if (!initState->isFinal)
  {
    finalStates.push_back(initState);
    initState->setFinal(true);
  }
}

void LogicalVA :: assign(std::string varName) {
  /* Extends the LogicalVA so it can assign its pattern to a variable */

  // TODO: Check if new var is not in the vars set of the LogicalVA

  // Create new states
  LVAState* openLVAState = new LVAState();
  LVAState* closeLVAState = new LVAState();

  // Add new states to states list
  states.push_back(openLVAState);
  states.push_back(closeLVAState);
  numStates = states.size();

  std::bitset<32> openCoding, closeCoding;
  openCoding = vFact->getOpenCode(varName);
  closeCoding = vFact->getCloseCode(varName);

  // Connect new open LVAState with init LVAState
  openLVAState->addCapture(openCoding, initState);

  // Set open LVAState as new init LVAState
  initState = openLVAState;

  // Connect final states with new close LVAState
  for(std::size_t i=0;i<finalStates.size();i++){
    finalStates[i]->addCapture(closeCoding, closeLVAState);
    finalStates[i]->setFinal(false);
  }

  // Set close LVAState as the only final LVAState
  finalStates.clear();
  finalStates.push_back(closeLVAState);
  closeLVAState->setFinal(true);
}

std::string LogicalVA :: pprint() {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the LogicalVA with the unique ids for
     each LVAState */


  // Declarations
  std::stringstream ss, cond;
  LVAState *current;
  int cid, nid;  // cid: current LVAState id; nid : next LVAState id
  std::bitset<32> S;



  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<LVAState*> queue;

  // Start on the init LVAState
  visited.insert(initState->id);
  queue.push_back(initState);



  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();
    cid = current->id;

    // For every epsilon transition
    for (auto &epsilon: current->e) {
      nid = epsilon->next->id;

      ss << "t " << cid << " eps " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(epsilon->next);
      }
    }

    // For every capture transition
    for (auto &capture: current->c) {
      S = capture->code;

      nid = capture->next->id;

      ss << "t " << cid << " " << vFact->getVarUtil(S) << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter: current->f) {
      nid = filter->next->id;
      S = filter->code;

      // TODO: Get the correct transition name

      ss << "t " << cid << ' ' << fFact->getFilter(filter->code).print() << ' ' << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }
  }

  // Code final states
  for (size_t i = 0; i < finalStates.size(); ++i) {
    if(finalStates[i]->isFinal) {
      ss << "f " << finalStates[i]->id << '\n';
    }
  }

  // Code initial LVAState
  ss << "i " << initState->id;

  return ss.str();
}