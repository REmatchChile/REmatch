#ifndef STRUCTS__ECS__ECS_HPP
#define STRUCTS__ECS__ECS_HPP

#include <bitset>
#include <sstream>
#include <string>
#include <vector>

#include "structs/ecs/mempool.hpp"

namespace rematch {

class ECS {
public:
  struct Data {
    Data(std::bitset<32> S, int64_t i) : S(S), i(i) {}
    Data() : S(0), i(0) {}
    std::bitset<32> S;
    int64_t i;
  };

  enum class NodeType { kBottom = 0, kUnion = 1, kLabel = 2 };

  class Node;

  class Node {
    friend class ECS;
   public:
    Node(NodeType t, Node *left = nullptr, Node *right = nullptr,
         std::bitset<32> S = 0, int64_t i = 0);

    bool is_output() const { return S_[S_.size() - 2]; }
    bool is_bottom() const { return S_[S_.size() - 2] && !S_[S_.size() - 1]; }
    Node *left() const { return left_; }
    Node *right() const { return is_output() ? nullptr : right_; }
    Node *next() const { return left_; };
    Data data() const {
      return !is_output() || is_bottom() ? Data() : Data(S_, i_);
    }
    // std::string print(int depth=0) const = 0;

    // Decrease the reference counter (for garbage collection)
    void dec_ref_count() { --ref_count_; }
    // Increase the reference counter (for garbage collection)
    void inc_ref_count() { ++ref_count_; }

    Node *reset(NodeType t, Node *left = nullptr, Node *right = nullptr,
                std::bitset<32> S = 0, int64_t i = 0);

    // friend std::ostream& operator<<(std::ostream& os, const Node& n);

    union {
      uint ref_count_{1};
      Node *next_free_;
    };

    std::bitset<32> S_;
    Node *left_ = nullptr;

    union {
      Node *right_ = nullptr;
      int64_t i_;
    };

    uint32_t id_;
    NodeType type_;

   private:
    static uint32_t NextID;

    uint reset_count_ = 0;

  }; // end class ECS::Node

  ECS() = default;

  Node *extend(Node *v, std::bitset<32> S, int64_t i);

  Node *unite(Node *v1, Node *v2, bool delay=false);

  Node *bottom_node() { return pool_.alloc(NodeType::kBottom); }

  // Marks the node as unused iff its ref_count_ == 0. Otherwise nothing
  // happens. The Node MUST be present in this->pool_.
  void try_mark_unused(Node *v) {
    #ifndef NOPT_MEMORY
    if (v->ref_count_ == 0)
      pool_.add_to_free_list(v);
    #endif
  }

  size_t n_nodes() const { return pool_.n_nodes(); }
  size_t n_reused_nodes() const { return pool_.n_reused_nodes(); }

private:
  MemPool<Node> pool_;

  std::vector<Node*> delay_check_unused_;

}; // end class ECS

} // end namespace rematch

#endif // STRUCTS__ECS__ECS_HPP