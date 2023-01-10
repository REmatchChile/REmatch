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
      int ref_count{1};
      ECSNode *next_free_node;
    };
    std::bitset<64> variable_markers;
    ECSNode *left = nullptr;
    ECSNode *right = nullptr;
    ECSNodeType type;

  private:
    int reset_count = 0;

  public:
    ECSNode(ECSNodeType node_type, ECSNode *left = nullptr,
            ECSNode *right = nullptr, std::bitset<64> variable_markers = 0);
    ECSNode *reset(
        ECSNodeType node_type, ECSNode *left = nullptr,
        ECSNode *right = nullptr, std::bitset<64> variable_markers = 0);

    bool is_output() const;
    bool is_bottom() const;

    ECSNode *left_node() const { return left; }
    ECSNode *right_node() const { return is_output() ? nullptr : right; }
    ECSNode *next() const { return left; };

    // TODO: Tal vez agregar un node container o
    //       enumerator que se encarga de los ref_counts
    void dec_ref_count() { --ref_count; }
    void inc_ref_count() { ++ref_count; }

    friend std::ostream& operator<<(std::ostream& os, const ECSNode& n);

 private:
  void assign_attributes(ECSNodeType node_type, ECSNode *left,
                     ECSNode *right, std::bitset<64> variable_markers);
  void reset_attributes();
  void label_node_as_kBottom();
  void create_kUnion_node(ECSNode *left, ECSNode *right);
  void label_node_as_kUnion();
  void create_kLabel_node(ECSNode *left, std::bitset<64> variable_markers);
  void label_node_as_kLabel();

}; // end class ECSNode

} // namespace rematch

#endif // STRUCTS_ECvariable_markers_NODE_HPP
