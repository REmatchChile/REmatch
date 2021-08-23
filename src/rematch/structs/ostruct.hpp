#ifndef STRUCTS__OSTRUCT
#define STRUCTS__OSTRUCT

#include <bitset>

namespace rematch {
namespace internal {

class OutputID;

class OStruct {
 public:
  virtual OutputID* extend(OutputID* oid, std::bitset<32> S, int64_t i) = 0;
  virtual OutputID* unite(OutputID* oid1, OutputID* oid2) = 0;
  virtual OutputID* enumerate(OutputID* oid) = 0;
  virtual OutputID* empty_oid() = 0;
}; // end class OStruct

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__OSTRUCT