#include "lva.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>
#include <cassert>
#include <map>

#include "automata/nfa/state.hpp"
#include "factories/factories.hpp"
#include "parse/parser.hpp"

namespace rematch {

// FIXME: Asume that L(A) doesn't have an epsilon. If it does then have 
// a boolean to set the special case.

// FIXME: Initial state only has outwards transitions, and final state only has
// inward transitions.

LogicalVA::LogicalVA()
    : init_state_(new State()),
      final_state_(new State()),
      vfactory_(std::make_shared<VariableFactory>()),
      ffactory_(std::make_shared<FilterFactory>()) {
  init_state_->setInitial(true);
  final_state_->setFinal(true);
  states.push_back(init_state_);
  states.push_back(final_state_);
}

LogicalVA::LogicalVA(uint code) : is_raw_(false) {
  init_state_ = new_state();
  init_state_->setInitial(true);

  final_state_ = new_state();
  final_state_->setFinal(true);

  init_state_->addFilter(code, final_state_);
}

LogicalVA::LogicalVA(const LogicalVA &A)
    : init_state_(nullptr),
      is_raw_(A.is_raw_),
      vfactory_(A.vfactory_),
      ffactory_(A.ffactory_) {

  // Table to map old states to newly allocated copied states
  std::unordered_map<State*, State*> new_states;

  // Prepare states vectors
  states.reserve(A.states.size());
  // Iterative Search using stack for cleanliness in function definitions,
  std::vector<std::pair<State*, State*>> stack;

  // First, make a copy of all states
  for(auto& q_old: A.states) {
    State* q_new = new State(*q_old);
    new_states[q_old] = q_new;
    states.push_back(q_new);
  }

  // Then start copying the transitions
  for(auto& q_old: A.states) {
    State* q_new = new_states[q_old];

    if(q_old->isFinal)
      q_new->addEpsilon(final_state_);

    for(auto& filt: q_old->filters)
      q_new->addFilter(filt->code, new_states[filt->next]);
    for(auto& cap: q_old->captures)
      q_new->addCapture(cap->code, new_states[cap->next]);
    for(auto& eps: q_old->epsilons)
      q_new->addEpsilon(new_states[eps->next]);
  }

  init_state_ = new_states[A.init_state_];
}

void LogicalVA::set_factories(std::shared_ptr<VariableFactory> v,
										          std::shared_ptr<FilterFactory>   f) {
  vfactory_ = v;
  ffactory_ = f;
}

void LogicalVA::adapt_capture_jumping() {
  std::vector<State*> stack;
  State *reached_state;

  for(auto &state: states) {
    for(auto &state2: states) {
      state2->tempMark = false;
    }
    stack.clear();

    for(auto &capture: state->captures) {
      stack.push_back(capture->next);
      state->tempMark = false;
    }

    while(!stack.empty()) {
      reached_state = stack.back();
      stack.pop_back();

      reached_state->tempMark = true;

      if(!reached_state->filters.empty() || !reached_state->epsilons.empty() || reached_state->isFinal)
        state->addEpsilon(reached_state);
      for(auto &capture: reached_state->captures) {
        if(!capture->next->tempMark)
          stack.push_back(capture->next);
      }
    }
  }

  for(auto &state: states)
    state->captures.clear();

  is_raw_ = true;
}

void LogicalVA::cat(LogicalVA &a2) {
  /* Concatenates an LogicalVA a2 to the current LogicalVA (inplace) */

  // Adds eps transitions from final states to a2 init State
  final_state_->addEpsilon(a2.init_state_);
  final_state_->setFinal(false);
  final_state_ = a2.final_state_;

  a2.init_state_->setInitial(false);

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());
}

void LogicalVA::alter(LogicalVA &a2) {
  /* Extends the LogicalVA so it can alternate between itself and an
     LogicalVA a2 */

  // Creates a new init State and connects it to the old init and a2's init
  State* newinitState = new_state();

  newinitState->addEpsilon(init_state_);
  newinitState->addEpsilon(a2.init_state_);

  init_state_->setInitial(false);

  init_state_ = newinitState;

  init_state_->setInitial(true);

  states.push_back(init_state_);

  // Add a2 final state to final states list
  State* new_accepting_state = new_state();

  final_state_->setFinal(false);
  a2.final_state_->setFinal(false);

  final_state_->addEpsilon(new_accepting_state);
  a2.final_state_->addEpsilon(new_accepting_state);

  final_state_ = new_accepting_state;
  a2.final_state_ = new_accepting_state;

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());
}

void LogicalVA::kleene() {
  State* ni = new_state();
  State* nf = new_state();
  final_state_->addEpsilon(init_state_);
  init_state_->addEpsilon(ni);
}

void LogicalVA::strict_kleene() {
  final_state_->addEpsilon(init_state_);
}

void LogicalVA :: optional() {
  init_state_->addEpsilon(final_state_);
}

void LogicalVA::assign(std::bitset<32> open_code, std::bitset<32> close_code) {
  /* Extends the LogicalVA so it can assign its pattern to a variable */

  // Create new states
  State* openLVAState = new_state();
  State* closeLVAState = new_state();

  // Connect new open State with init State
  openLVAState->addCapture(open_code, init_state_);

  init_state_->setInitial(false);

  // Set open State as new init State
  init_state_ = openLVAState;

  init_state_->setInitial(true);

  final_state_->addCapture(close_code, closeLVAState);
  final_state_->setFinal(false);

  final_state_ = closeLVAState;

  final_state_->setFinal(true);
}

void LogicalVA::repeat(int min, int max) {
  // std::cout << pprint() << "\n\n";
  LogicalVA copied(*this);
  if (min == 0 && max == -1) {
    kleene(); return;
  }

  for(int i=1; i < min-1; i++) {
    LogicalVA copied_automaton(copied);
    cat(copied_automaton);
  }

  if(min > 1) {
    LogicalVA copied_automaton(copied);
    if(max == -1)
      copied_automaton.strict_kleene();
    cat(copied_automaton);
  }

  // This is the optional part
  if(max-min > 0) {
    LogicalVA *copied_automaton, *copied_automaton2;
    if(min == 0) {
      if(max > 1) {
        copied_automaton = new LogicalVA(copied);
        copied_automaton->optional();
        for(int i=min+1; i < max-1; i++) {
          copied_automaton2 = new LogicalVA(copied);
          copied_automaton2->cat(*copied_automaton);
          copied_automaton2->optional();
          copied_automaton = copied_automaton2;
       }
       cat(*copied_automaton);
       optional();
      } else {
        optional();
      }
    } else {
      copied_automaton = new LogicalVA(copied);
      copied_automaton->optional();
      for(int i=min+1; i < max; i++) {
        copied_automaton2 = new LogicalVA(copied);
        copied_automaton2->cat(*copied_automaton);
        copied_automaton2->optional();
        copied_automaton = copied_automaton2;
      }
      cat(*copied_automaton);
    }
  }

  //  std::cout << pprint() << "\n\n";
}

std::string LogicalVA :: pprint() {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the LogicalVA with the unique ids for
     each State */


  // Declarations
  std::stringstream ss, cond;
  State *current;
  int cid, nid;  // cid: current State id; nid : next State id
  std::bitset<32> S;

  std::vector<int> capture_states, pcapture_states;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<State*> queue;

  // Start on the init State
  visited.insert(init_state_->id);
  queue.push_back(init_state_);

  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();
    cid = current->id;

    if(current->flags() & State::kCaptureState)
			capture_states.push_back(current->id);
    if(current->flags() & State::kPreCaptureState)
			pcapture_states.push_back(current->id);

    // For every epsilon transition
    for (auto &epsilon: current->epsilons) {
      nid = epsilon->next->id;

      ss << "t " << cid << " eps " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(epsilon->next);
      }
    }

    // For every capture transition
    for (auto &capture: current->captures) {
      S = capture->code;

      nid = capture->next->id;

      ss << "t " << cid << " " << vfactory_->print_varset(S) << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter: current->filters) {
      nid = filter->next->id;
      S = filter->code;

      // TODO: Get the correct transition name

      ss << "t " << cid << ' ' << ffactory_->get_filter(filter->code) << ' ' << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }
  }

  // Code final states
  ss << "f " << final_state_->id << '\n';

  // Code initial State
  ss << "i " << init_state_->id;

  return ss.str();
}

State* LogicalVA::new_state() {
  State *nstate = new State();
  states.push_back(nstate);

  return nstate;
}

} // end namespace rematch