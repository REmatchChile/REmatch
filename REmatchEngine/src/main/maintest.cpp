#include "wasminterface.hpp"
#include "cli.hpp"

int main(int argc, char const *argv[])
{
   WasmInterface wasm(argv[1], argv[2]);

   std::cout << "In this while.\n";

    for(auto &var: wasm.getOutputSchema()) {
        std::cout << "Var: " << var << "\n";
    }

    while(wasm.hasNext()) {
        std::cout << "{";
        for(auto &elem: wasm.next()) {
            std::cout << '|' << elem.x << ',' << elem.y << ">, ";
        }
        std::cout << "}\n";
    }

    return 0;
}


