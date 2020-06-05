#ifdef SWIG

// Rematch interface
%module rematch

%include <std_string.i>
%include <std_pair.i>

%{
#include "interfaces/pyinterface.hpp"
using namespace rematch;
%}

%inline %{
typedef long unsigned int size_t;
%}

%template(PairSize) std::pair<size_t,size_t>;

%include "match.hpp"
%include "interfaces/pyinterface.hpp"

#endif // SWIG
