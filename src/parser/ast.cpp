#include "ast.hpp"

// These are the definitions for the stream operator on ast objects. It's
// required by Boost Spirit's debugging.

namespace rematch {
namespace ast {


std::ostream& operator<<(std::ostream& os, const parenthesis& p) {
    os << "()";
    return os;
}

} // end namespace ast
} // end namespace rematch