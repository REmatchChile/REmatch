#include "wasminterface.hpp"
#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "evaluation.hpp"


// Make VSCode Linter ignore the include error
#ifdef __EMSCRIPTEN__
    #include <emscripten/bind.h>
#endif

// typedef std::map<std::string, std::pair<size_t, size_t>> mapping_type;

WasmInterface :: WasmInterface(std::string doc, std::string regex)
    : documentStream(new std::istringstream(doc)), regex(regex) {

    LogicalVA A = regex2LVA(regex);
	ExtendedVA *B = new ExtendedVA(A);
    std::stringstream ss;
    evaluation = new Evaluation(B, *documentStream, ss, false, false);
    }

std::vector<std::string> WasmInterface :: getOutputSchema() {
    return evaluation->getOutputSchema();
}
void WasmInterface :: init() {}

bool WasmInterface :: hasNext() {
    return evaluation->hasNext00();
}

std::vector<pair_> WasmInterface :: next() {
    auto mapping = evaluation->next();
    std::vector<pair_> ret;
    for(auto &elem: mapping) {
        ret.emplace_back(elem.second);
    }
    return ret;
}

// Make VSCode Linter ignore the include error
#ifdef __EMSCRIPTEN__

EMSCRIPTEN_BINDINGS(my_class_example) {
  emscripten::register_vector<std::string>("VectorStr");
  emscripten::register_vector<pair_>("VectorPair");

  emscripten::value_array<pair_>("pair_")
     .element(&pair_::x)
     .element(&pair_::y)
     ;

  emscripten::class_<WasmInterface>("WasmInterface")
    .constructor<std::string, std::string>()
    .function("getOutputSchema", &WasmInterface::getOutputSchema)
    .function("init", &WasmInterface::init)
    .function("next", &WasmInterface::next)
    .function("hasNext", &WasmInterface::hasNext)
    // .function("pairConvert", &WasmInterface::pairConvert)
    ;
}

#endif

