#ifndef STRUCTvariable_markers_ECvariable_markers_NODE_HPP
#define STRUCTvariable_markers_ECvariable_markers_NODE_HPP

#include <cinttypes>
#include <bitset>

namespace rematch {
enum class ECSNodeType { kBottom = 0, kUnion = 1, kLabel = 2 };
class ECSNode {
  friend class ECS;

  public:
    union {
      int ref_count_{1};
      ECSNode *next_free_;
    };
    std::bitset<32> variable_markers_;
    ECSNode *left_ = nullptr;
    ECSNode *right_ = nullptr;
    ECSNodeType type_;

  private:
    int reset_count_ = 0;

  public:
    ECSNode(ECSNodeType node_type, ECSNode *left = nullptr,
            ECSNode *right = nullptr, std::bitset<32> variable_markers = 0);

    bool is_output() const;
    bool is_bottom() const;
    ECSNode *left() const { return left_; }
    ECSNode *right() const { return is_output() ? nullptr : right_; }
    ECSNode *next() const { return left_; };

  void dec_ref_count() { --ref_count_; }
  void inc_ref_count() { ++ref_count_; }

  ECSNode *reset(
      ECSNodeType node_type, ECSNode *left = nullptr,
      ECSNode *right = nullptr, std::bitset<32> variable_markers = 0);

  // friend std::ostream& operator<<(std::ostream& os, const Node& n);

 private:
  void assign_attributes(ECSNodeType node_type, ECSNode *left,
                     ECSNode *right, std::bitset<32> variable_markers);
  void reset_attributes();
  void label_node_as_kBottom();
  void create_kUnion_node(ECSNode *left, ECSNode *right);
  void label_node_as_kUnion();
  void create_kLabel_node(ECSNode *left, std::bitset<32> variable_markers);
  void label_node_as_kLabel();

}; // end class ECSNode

}
     //
#endif // STRUCTS_ECvariable_markers_NODE_HPP
