#ifndef SRC_REMATCH_LOGICAL_VA_STATE_HPP
#define SRC_REMATCH_LOGICAL_VA_STATE_HPP

#include <list>
#include <set>
#include <bitset>
#include <memory>

#include "parsing/charclass.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_capture.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_filter.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_epsilon.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_anchor.hpp"

namespace REmatch {
inline namespace parsing {

class LogicalVAState {

  enum LogicalVAStateFlags {
    kDefaultLogicalVAState = 0,
    kFinalLogicalVAState = 1,
    kSuperFinalLogicalVAState = 1 << 1,
    kInitialLogicalVAState = 1 << 2,
    kStartAnchorLogicalVAState = 1 << 3,
    kEndAnchorLogicalVAState = 1 << 4,
    kCaptureLogicalVAState = 1 << 5,
    kPreCaptureLogicalVAState = 1 << 6
  };

  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    unsigned int id;

    std::list<LogicalVAFilter*> filters;
    std::list<LogicalVACapture*> captures;
    std::list<LogicalVAEpsilon*> epsilons;
    std::list<LogicalVAAnchor*> anchors;

    std::list<LogicalVACapture*> backward_captures_;
    std::list<LogicalVAFilter*> backward_filters_;
    std::list<LogicalVAEpsilon*> backward_epsilons_;
    std::list<LogicalVAAnchor*> backward_anchors_;

    /// For graph algorithms
    bool tempMark = false;
    /// For graph algorithms
    char colorMark = 'w';
    /// For trimming
    unsigned int visited_and_useful_marks = 0;

    unsigned int flags;

    LogicalVAState();

    ~LogicalVAState();

    LogicalVAState(const LogicalVAState& s);

    void add_capture(std::bitset<64> code, LogicalVAState* next);
    void add_filter(CharClass charclass, LogicalVAState* next);
    void add_epsilon(LogicalVAState* next);
    void add_anchor(bool is_start, LogicalVAState* next);

    // Getters and setters
    bool initial() const { return flags & kInitialLogicalVAState; }
    void set_initial(bool b);

    bool accepting() const { return flags & kFinalLogicalVAState; }
    void set_accepting(bool b);

    bool super_final() const { return flags & kSuperFinalLogicalVAState; }

    unsigned int get_flags() const {return flags;}
    void set_flags(unsigned int f) {flags = f;}

    bool operator==(const LogicalVAState &rhs) const;
};

} // end namespace REmatch
}

#endif
