#ifdef SWIG

// Rematch interface
%module rematch

%include <std_string.i>
%include <std_pair.i>

%{
#include "regex/regex.hpp"
using namespace rematch;
%}

%inline %{
typedef long unsigned int size_t;
%}

%template(PairSize) std::pair<size_t,size_t>;

%include "match.hpp"
%include "regex/regex.hpp"
%include "regex/regex_options.hpp"

#endif // SWIG
