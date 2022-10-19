#ifndef AUTOMATA_ABS_DSTATE_HPP
#define AUTOMATA_ABS_DSTATE_HPP

#include <optional>

#include "automata/dfa/transition.hpp"
#include "structs/ecs/ecs.hpp"

namespace rematch {

namespace abstract {

class DState {
 public:
  virtual ECS::Node* node() const = 0;
  virtual void set_node(ECS::Node* n) = 0;

  virtual bool accepting() const = 0;
  virtual bool initial() const = 0;

  virtual int visited() const = 0;
  virtual void set_visited(int n) = 0;

}; // end class DState

} // end namespace abstract

} // end namespace rematch

#endif // AUTOMATA_ABS_DSTATE_HPP
