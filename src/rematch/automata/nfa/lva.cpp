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

LogicalVA::LogicalVA()
    : init_state_(new_state()),
      final_state_(new_state()),
      vfactory_(std::make_shared<VariableFactory>()),
      ffactory_(std::make_shared<FilterFactory>()) {
  init_state_->set_initial(true);
  final_state_->set_accepting(true);
  states.push_back(init_state_);
  states.push_back(final_state_);
}

LogicalVA::LogicalVA(uint code) {
  init_state_ = new_state();
  init_state_->set_initial(true);

  final_state_ = new_state();
  final_state_->set_accepting(true);

  init_state_->add_filter(code, final_state_);
}

LogicalVA::LogicalVA(const LogicalVA &A)
    : init_state_(nullptr),
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

    for(auto& filt: q_old->filters)
      q_new->add_filter(filt->code, new_states[filt->next]);
    for(auto& cap: q_old->captures)
      q_new->add_capture(cap->code, new_states[cap->next]);
  }

  init_state_ = new_states[A.init_state_];
  final_state_ = new_states[A.final_state_];
}

void LogicalVA::set_factories(std::shared_ptr<VariableFactory> v,
										          std::shared_ptr<FilterFactory>   f) {
  vfactory_ = v;
  ffactory_ = f;
}

void LogicalVA::trim() {
  // We'll do a simple BFS from the initial and final states (using backwards
  // transitions), storing the states that are reached by both procedures

  for(auto &p: states)
    p->visitedBy = 0;

  // Marked constants
  const int kReachable  = 1 << 0;  // 0000 0000 0000 0001
  const int kUseful     = 1 << 1;  // 0000 0000 0000 0010

  std::vector<State*> trimmed_states;  // New states vector
  std::deque<State*> queue;

  // Start the search forward from the initial state.
  queue.push_back(init_state_);
  init_state_->visitedBy |= kReachable;

  while(!queue.empty()) {
    State* p = queue.front(); queue.pop_front();

    for(auto &f: p->filters) {
      if(!(f->next->visitedBy & kReachable)) {
        f->next->visitedBy |= kReachable;
        queue.push_back(f->next);
      }
    }

    for(auto &c: p->captures) {
      if(!(c->next->visitedBy & kReachable)) {
        c->next->visitedBy |= kReachable;
        queue.push_back(c->next);
      }
    }
  }

  // Now start the search backwards from the final state.
  queue.push_back(final_state_);
  final_state_->visitedBy |= kUseful;

  while(!queue.empty()) {
    State* p = queue.front(); queue.pop_front();
    for(auto &f: p->incidentFilters) {
      if(!(f->from->visitedBy & kUseful)) {
        if(f->from->visitedBy & kReachable)
          trimmed_states.push_back(f->from);
        f->from->visitedBy |= kUseful;
        queue.push_back(f->from);
      }
    }
    for(auto &c: p->incident_captures_) {
      if(!(c->from->visitedBy & kUseful)) {
        if(c->from->visitedBy & kReachable)
          trimmed_states.push_back(c->from);
        c->from->visitedBy |= kUseful;
        queue.push_back(c->from);
      }
    }
  }

  trimmed_states.push_back(final_state_);

  // Delete useless states
  for(auto *p: states) {
    if(p->visitedBy != (kReachable | kUseful))
      delete p;
  }

  states.swap(trimmed_states);
}

void LogicalVA::cat(LogicalVA &a2) {
  /* Concatenates an LogicalVA a2 to the current LogicalVA (inplace) */

  // Connect transitions from a1.final_state to reached states of a2.init_state
  for(auto &f: a2.init_state_->filters)
    final_state_->add_filter(f->code, f->next);

  for(auto &c: a2.init_state_->captures)
    final_state_->add_capture(c->code, c->next);

  a2.init_state_->set_initial(false);
  final_state_->set_accepting(false);

  final_state_ = a2.final_state_;

  // Add a2 states to state list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  has_epsilon_ = has_epsilon_ && a2.has_epsilon_;
}

void LogicalVA::alter(LogicalVA &a2) {
  /* Extends the LogicalVA so it can alternate between itself and an
     LogicalVA a2 */

  for(auto &f: a2.init_state_->filters) {
    if(!f->next->accepting())
      init_state_->add_filter(f->code, f->next);
    else
      init_state_->add_filter(f->code, final_state_);
  }
  for(auto &c: a2.init_state_->captures) {
    if(!c->next->accepting())
      init_state_->add_capture(c->code, c->next);
    else
      init_state_->add_capture(c->code, final_state_);
  }

  for(auto &f: a2.final_state_->incidentFilters) {
    if(!f->from->initial()) // Prevent duplicated transitions
      f->from->add_filter(f->code, final_state_);
  }

  for(auto &c: a2.final_state_->incident_captures_) {
    if(!c->from->initial()) // Prevent duplicated transitions
      c->from->add_capture(c->code, final_state_);
  }

  a2.init_state_->set_initial(false);
  a2.final_state_->set_accepting(false);

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  has_epsilon_ = has_epsilon_ || a2.has_epsilon_;
}

void LogicalVA::kleene() {
  strict_kleene();
  optional();
}

void LogicalVA::strict_kleene() {
  State* q = new_state();

  for(auto &f: init_state_->filters)
    q->add_filter(f->code, f->next);
  for(auto &c: init_state_->captures)
    q->add_capture(c->code, c->next);

  for(auto &f: final_state_->incidentFilters) {
    f->from->add_filter(f->code, q);
    if(f->from->initial())
      q->add_filter(f->code, q);
  }
  for(auto &c: final_state_->incident_captures_)
    c->from->add_capture(c->code, q);
}

void LogicalVA :: optional() {
  has_epsilon_ = true;
}

void LogicalVA::assign(std::bitset<32> open_code, std::bitset<32> close_code) {
  /* Extends the LogicalVA so it can assign its pattern to a variable */

  // Create new states
  State* openLVAState = new_state();
  State* closeLVAState = new_state();

  // Connect new open State with init State
  openLVAState->add_capture(open_code, init_state_);

  init_state_->set_initial(false);

  // Set open State as new init State
  init_state_ = openLVAState;

  init_state_->set_initial(true);

  final_state_->add_capture(close_code, closeLVAState);
  final_state_->set_accepting(false);

  final_state_ = closeLVAState;

  final_state_->set_accepting(true);
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

void LogicalVA::remove_captures() {
  std::vector<State*> stack;
  stack.reserve(states.size());

  for(auto &q: states)
    q->visitedBy = 0;

  for(auto &r: states) {
    if(r->captures.empty() || (r->incidentFilters.empty() && !r->initial()))
      continue;

    stack.clear();

    r->visitedBy = r->id;
    stack.push_back(r);

    while(!stack.empty()) {
      State* p = stack.back(); stack.pop_back();

      for(auto& capt: p->captures) {
        State* q = capt->next;
        if(q->visitedBy != r->id) {
          q->visitedBy = r->id;

          for(auto &filt: q->filters)
            r->add_filter(filt->code, filt->next);

          if(!q->captures.empty())
            stack.push_back(q);
        }
      }
    }
  }

  // Do the same for the accepting state, but backwards
  if(!final_state_->incident_captures_.empty()) {
    stack.clear();
    final_state_->visitedBy = final_state_->id;
    stack.push_back(final_state_);

    while(!stack.empty()) {
      State* p = stack.back(); stack.pop_back();

      for(auto& capt: p->incident_captures_) {
        State* q = capt->from;
        if(q->visitedBy != final_state_->id) {
          q->visitedBy = final_state_->id;

          for(auto &filt: q->incidentFilters)
            filt->from->add_filter(filt->code, final_state_);

          if(!q->incident_captures_.empty())
            stack.push_back(q);
        }
      }
    }
  }

  for(auto &p: states) {
    p->captures.clear();
    p->incident_captures_.clear();
  }

  trim();
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
    // for (auto &epsilon: current->epsilons) {
    //   nid = epsilon->next->id;

    //   ss << "t " << cid << " eps " << nid << '\n';

    //   // If not visited enqueue and add to visited
    //   if (visited.find(nid) == visited.end()) {
    //     visited.insert(nid);
    //     queue.push_back(epsilon->next);
    //   }
    // }

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