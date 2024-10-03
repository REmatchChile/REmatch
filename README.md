# REmatch

Here you can find the main implementation of REmatch library in C++, and its bindings for both Python and JavaScript/WebAssembly. This version has been refactorized, tested, and developed for being ready for production.

## Directory structure

- `/src`: The C++ implementation.
- `/tests`: Contains all automatic tests for the code.
- `/scripts`: Contains useful scripts for setting up the project, including compilation and other tasks.

## Build instructions

### Setup

Dependencies:

- Git
- Clang version 11 or newer
- CMake version 3.23.2 or newer
- Catch2 (required for tests only)

On current Debian and Ubuntu based distributions this dependencies can be installed with the following command:

```bash
# Dependencies for REmatch
sudo apt update
sudo apt install git clang cmake

# Install Catch2 (required for tests only)
git clone https://github.com/catchorg/Catch2.git
cd Catch2/
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
cd ..
rm -rf Catch2
```

### Building

The target executable file is located at `src/targets/bin/rematch.cpp`. To build, run:

```bash
cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release && cmake --build build/Release/
```

After building, the binary file will be located in the `build/Release/bin` folder. To try it simply run:

```bash
./build/Release/bin/rematch
```

If you want to use a debugger such as `gdb`, then you should run the following command:

```bash
cmake -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug && cmake --build build/Debug/
```

The binary path instead will be at `build/Debug/bin`.

## Examples

To use REmatch, you have two options. You can create a `Query` object through the method `REMatch::compile` and pass the regular expression as argument, or you can directly call the functions provided.

```cpp
// Compile a regular expression using the compile method and find a match
REMatch::Query query = REMatch::compile("!x{aba}");
std::unique_ptr<REMatch::Match> query_match = query.findone("aabaa");

// Equivalent to calling findone directly
std::unique_ptr<REMatch::Match> direct_match = REMatch::findone("!x{aba}", "aabaa");
```

The `Query` provides the methods `findone` and `finditer` that evaluate a document and return the matches found. The `findone` method returns a pointer to the first encountered match, while `finditer` returns an iterator that allows you to access all matches. To retrieve all the matches at once, you can use the `findall` method. You can use the `start` and `end` methods to obtain the indices of the matched spans or `span` to get a string representation.

### Retrieving an iterator for the pattern aba

```cpp
#include <memory>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "abaababa";
  REMatch::Query query = REMatch::compile("!x{aba}");
  std::unique_ptr<REMatch::MatchIterator> iterator = query.finditer(document);
  std::unique_ptr<REMatch::Match> match = iterator->next();
  while (match != nullptr) {
    std::cout << "Span: [" << match->start("x") << ", " << match->end("x") << ">" << std::endl;
    match = iterator->next();
  }
  return 0;
}
```

### Finding the first match for the pattern aba

```cpp
#include <iostream>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "abaababa";
  REMatch::Query query = REMatch::compile("!x{aba}");
  std::unique_ptr<REMatch::Match> match = query.findone(document);
  std::cout << "Span: " << match->span("x") << std::endl;
  return 0;
}
```

### Finding a vector with all matches for the pattern aba

```cpp
#include <iostream>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "aba";
  std::string query = "!x{aba}";
  std::vector<REMatch::Match> matches = REMatch::library_interface::findall(query, document);
  for (REMatch::Match& match: matches) {
    std::cout << "Span: [" << match.start("x") << ", " << match.end("x") << ">" << std::endl;
  }
  return 0;
}
```

## Testing

We are using Catch2 for unit testing. To add more tests, create files at: `tests/<module_name>/<class_tested>.cpp`.

To build REmatch testing executable, run:

```bash
cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=1 && cmake --build build/Release/
```

Then you can run:

```bash
./build/Release/bin/rematch_test
```

## Profiler

We are using the Tracy profiler to measure the time spent in specific code segments. To profile the code using the graphical user interface, follow these steps:

1. Compile with the following command:

```bash
cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release -DENABLE_PROFILING=1 && cmake --build build/Release/
```

2. Navigate to `build/Release/_deps/tracy-src/profiler/build/unix` and run `make` to generate the "Tracy-release" executable.

3. Execute the "Tracy-release" executable.

4. Run REmatch to initiate the profiling.

You should be able to view the results in the graphical interface.

## Bindings

### JavaScript/WASM

To begin, follow the official installation tutorial for Emscripten available at <https://emscripten.org/docs/getting_started/downloads.html>

Once the installation is complete, execute the following command:

```bash
emcmake cmake -Bbuild/javascript -DCMAKE_BUILD_TYPE=Release && cmake --build build/javascript/
```

Then the JavaScript/WASM bindings can be found in `./build/javascript/bin/`

## Python

To install the latest release version from PyPI, run:

```bash
pip install pyrematch
```

To build from the source code, clone this repository and run:

```bash
pip install .
```

## Reference

This implementation is based on the paper [REmatch: a novel regex engine for finding all matches](https://www.vldb.org/pvldb/vol16/p2792-vrgoc.pdf) by Cristian Riveros, Nicolás Van Sint Jan, and Domagoj Vrgoč.
