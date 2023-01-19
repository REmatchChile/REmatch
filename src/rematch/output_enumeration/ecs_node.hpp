#ifndef OUTPUT_ENUMERATION__NODE_HPP
#define OUTPUT_ENUMERATION__NODE_HPP

#include <cinttypes>
#include <bitset>

namespace rematch {

inline namespace output_enumeration {

enum class ECSNodeType { kBottom = 0, kUnion = 1, kLabel = 2 };

class ECSNode {
  friend class NodeManager;

  public:
    std::bitset<64> variable_markers;
    ECSNode *left = nullptr;
    union {
      ECSNode *right = nullptr;
      int document_position;
    };
    ECSNodeType type;

  private:
    union {
      int ref_count{1};
      ECSNode *next_free_node;
    };
    int reset_count = 0;

  public:
    ECSNode(ECSNodeType node_type,
            ECSNode *left = nullptr, ECSNode *right = nullptr,
            std::bitset<64> variable_markers = 0,
            int document_position = 0);
    ECSNode *reset(ECSNodeType node_type,
                   ECSNode *left = nullptr, ECSNode *right = nullptr,
                   std::bitset<64> variable_markers = 0,
                   int document_position = 0);

    bool is_output() const;
    bool is_bottom() const;

    ECSNode *left_node() const { return left; }
    ECSNode *right_node() const { return is_output() ? nullptr : right; }
    ECSNode *next() const { return left; };

    // TODO: Tal vez agregar un node container o
    //       enumerator que se encarga de los ref_counts

    friend std::ostream& operator<<(std::ostream& os, const ECSNode& n);

 private:
  void decrease_ref_count() { --ref_count; }
  void increase_ref_count() { ++ref_count; }

  void assign_attributes(ECSNodeType node_type, ECSNode *left,
                     ECSNode *right, std::bitset<64> variable_markers,
                     int document_position);
  void reset_attributes();
  void label_node_as_kBottom();
  void create_kUnion_node(ECSNode *left, ECSNode *right);
  void add_left_node(ECSNode *node);
  void add_right_node(ECSNode *node);
  void label_node_as_kUnion();
  void create_kLabel_node(ECSNode *left, std::bitset<64> variable_markers,
                          int document_position);
  void label_node_as_kLabel();

}; // end class ECSNode

} // end namespace output_enumeration

} // end namespace rematch

#endif // OUTPUT_ENUMERATION__NODE_HPP
