#ifndef STRUCTS__ECS__ECS_HPP
#define STRUCTS__ECS__ECS_HPP

#include <bitset>
#include <vector>
#include <string>
#include <sstream>

#include "structs/ecs/mempool.hpp"

namespace rematch {

namespace internal {

class ECS {
 public:

  struct Data {
    Data(std::bitset<32> S, int64_t i) : S(S), i(i) {}
    Data(): S(0), i(0) {}
    std::bitset<32> S;
    int64_t i;
  };

  enum class NodeType { kBottom = 0, kUnion = 1, kLabel = 2};

  class Node;

  class Node {
   public:

    Node(NodeType t, Node* left=nullptr, Node* right=nullptr,
         std::bitset<32> S=0, int64_t i=0);

    bool is_output() const { return S_[S_.size()-2]; }
    bool is_empty() const { return S_[S_.size()-2] && !S_[S_.size()-1]; }
    Node* left() const { return left_; }
    Node* right() const { return is_output() ? nullptr : right_; }
    Node* next() const { return left_;};
    Data data() const { return !is_output() || is_empty() ? Data() : Data(S_, i_);}
    // std::string print(int depth=0) const = 0;

    Node* reset(NodeType t, Node* left=nullptr, Node* right=nullptr,
         std::bitset<32> S=0, int64_t i=0);

    union {
      uint ref_count_;
      Node* next_free_;
    };

    std::bitset<32> S_;
    Node* left_ = nullptr;

    union {
      Node* right_ = nullptr;
      int64_t i_;
    };

  };

  ECS() = default;

  Node* extend(Node* v, std::bitset<32> S, int64_t i);

  Node* unite(Node* v1, Node* v2);

  Node* empty_node() const { return new Node(NodeType::kBottom); }

  void mark_unused(Node* v);

 private:
  MemPool<Node> pool_;

}; // end class ECS

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__ECS__ECS_HPP