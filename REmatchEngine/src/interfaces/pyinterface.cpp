#include "pyinterface.hpp"
#include "parser/parser.hpp"

namespace rematch {

PyInterface::PyInterface(const std::string &doc, std::string regex) {
  LogicalVA A = regex2LVA(regex);
  LogicalVA auxA = regex2LVA(regex);
  ExtendedVA *B = new ExtendedVA(A);
  ExtendedVA *auxB = new ExtendedVA(auxA);
  std::stringstream ss;
  std::istringstream documentStream(doc);
  evaluation_ = new Evaluation(B, auxB, documentStream, ss, false, false);
}

Match PyInterface::next() {
  return evaluation_->next();
}

bool PyInterface::hasNext() {
  return evaluation_->hasNext00();
}

} // end namespace rematch