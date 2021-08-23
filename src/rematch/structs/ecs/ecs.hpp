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

  struct Node {
    virtual bool is_output() const = 0;
    virtual bool is_empty() const = 0;
    virtual Node* left() const = 0;
    virtual Node* right() const = 0;
    virtual Node* next() const = 0;
    virtual Data data() const = 0;
    virtual std::string print(int depth=0) const = 0;

    union {
      uint ref_count_ = 0; // Default initialized
      Node* next_free_;
    };
  };

  struct UnionNode: public Node {
    UnionNode(Node* l, Node* r): left_(l), right_(r) {}

    virtual bool is_output() const {return false;}
    virtual bool is_empty() const {return false;}

    virtual Node* left() const { return left_; }
    virtual Node* right() const { return right_; }

    virtual Node* next() const { return nullptr; }

    virtual Data data() const { return Data();}

    virtual std::string print(int depth=0) const {
      std::stringstream ss;
      std::string space(depth, ' ');
      ss << space << "<union>\n" << left_->print(depth+1)
                                 << right_->print(depth+1)
         << space <<  "</union>\n";
      return ss.str();
    }

    // 2*sizeof(Node*)

    Node* left_;
    Node* right_;
  }; // end struct Node

  struct EmptyNode: public Node {
    virtual bool is_output() const {return true;}
    virtual bool is_empty() const {return true;}

    virtual Node* left() const { return nullptr; }
    virtual Node* right() const { return nullptr; }

    virtual Node* next() const { return nullptr; }

    virtual Data data() const { return Data();}

    virtual std::string print(int depth=0) const {
      std::stringstream ss;
      std::string space(depth, ' ');
      ss << space << "<empty>\n";
      return ss.str();
    }

  }; // end struct EmptyNode

  struct LabelNode: public Node {
    LabelNode(std::bitset<32> S, int64_t i)
      : S(S), i(i), next_(nullptr) {}

    virtual bool is_output() const {return true;}
    virtual bool is_empty() const {return false;}

    virtual Node* left() const { return nullptr; }
    virtual Node* right() const { return nullptr; }

    virtual Node* next() const { return next_; }

    virtual Data data() const { return Data(S, i); }

    virtual std::string print(int depth=0) const {
      std::stringstream ss;
      std::string space(depth, ' ');
      ss << space << "<label S=(" << S << ") i=" << i << ">\n" << next_->print(depth+1)
         << space <<  "</label>\n";
      return ss.str();
    }

    std::bitset<32> S;
    int64_t i;
    Node* next_;
  }; // end struct Node

  ECS() = default;

  Node* extend(Node* v, std::bitset<32> S, int64_t i);

  Node* unite(Node* v1, Node* v2);

  Node* empty_node() const { return new EmptyNode; }

  void mark_unused(Node* v);

 private:
  MemPool<LabelNode> label_pool_;
  MemPool<UnionNode> union_pool_;

}; // end class ECS

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__ECS__ECS_HPP