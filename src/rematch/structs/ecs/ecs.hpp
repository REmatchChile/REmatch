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
    bool is_empty() const { return S_[S_.size()-1]; }
    Node* left() const { return left_; }
    Node* right() const { return is_output() ? nullptr : right_; }
    Node* next() const { return left_;};
    Data data() const { return !is_output() || is_empty() ? Data() : Data(S_, i_);}
    // std::string print(int depth=0) const = 0;

    union {
      uint ref_count_ = 0; // Default initialized
      Node* next_free_;
    };

    std::bitset<32> S_;
    Node* left_;

    union {
      int64_t i_;
      Node* right_;
    };

  };

  // struct UnionNode: public Node {
  //   UnionNode(Node* l, Node* r): left_(l), right_(r) {}

  //   virtual bool is_output() const {return false;}
  //   virtual bool is_empty() const {return false;}

  //   virtual Node* left() const { return left_; }
  //   virtual Node* right() const { return right_; }

  //   virtual Node* next() const { return nullptr; }

  //   virtual Data data() const { return Data();}

  //   virtual std::string print(int depth=0) const {
  //     std::stringstream ss;
  //     std::string space(depth, ' ');
  //     ss << space << "<union>\n" << left_->print(depth+1)
  //                                << right_->print(depth+1)
  //        << space <<  "</union>\n";
  //     return ss.str();
  //   }

  //   // 2*sizeof(Node*)

  //   Node* left_;
  //   Node* right_;
  // }; // end struct Node

  // struct EmptyNode: public Node {
  //   virtual bool is_output() const {return true;}
  //   virtual bool is_empty() const {return true;}

  //   virtual Node* left() const { return nullptr; }
  //   virtual Node* right() const { return nullptr; }

  //   virtual Node* next() const { return nullptr; }

  //   virtual Data data() const { return Data();}

  //   virtual std::string print(int depth=0) const {
  //     std::stringstream ss;
  //     std::string space(depth, ' ');
  //     ss << space << "<empty>\n";
  //     return ss.str();
  //   }

  // }; // end struct EmptyNode

  // struct LabelNode: public Node {
  //   LabelNode(std::bitset<32> S, int64_t i)
  //     : S(S), i(i), next_(nullptr) {}

  //   virtual bool is_output() const {return true;}
  //   virtual bool is_empty() const {return false;}

  //   virtual Node* left() const { return nullptr; }
  //   virtual Node* right() const { return nullptr; }

  //   virtual Node* next() const { return next_; }

  //   virtual Data data() const { return Data(S, i); }

  //   virtual std::string print(int depth=0) const {
  //     std::stringstream ss;
  //     std::string space(depth, ' ');
  //     ss << space << "<label S=(" << S << ") i=" << i << ">\n" << next_->print(depth+1)
  //        << space <<  "</label>\n";
  //     return ss.str();
  //   }

  //   std::bitset<32> S;
  //   int64_t i;
  //   Node* next_;
  // }; // end struct Node

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