# REmatch: An information extraction tool for regular document spanners

This implementation is based on the paper `REmatch: a regex engine
for finding all matches` by Cristian Riveros, Nicolar Van Sint Jan and
Domagoj Vrgoč.

## Directory structure

* `/src`: The C++ implementation.
* `/tests`: Contains all automatic tests for the code.
* `/scripts`: Contains useful scripts for setting up the project, including compilation and other tasks.

## Build instructions

### Setup

Dependencies:

* G++ version 11
* CMake version 3.23.2 or newer
* Boost version 1.58 or newer
* Catch2 (required for tests only)

The setup of CMake, Catch2, boost and g++-11 on a clean ubuntu system can be done using the following script:

```bash
./setup/ubuntu/setup_cpp_to_execute_rematch.sh
```

### Building

The target file is `rematch.cpp`, which should be located in the `src/targets/main/` directory. To build, run:

```bash
./scripts/compile.sh
```

After building, the binary file will be located in the `build/Release/bin` folder. To try it simply run:

```bash
./build/Release/bin/rematch
```

If you want to use a debugger such as `gdb`, then you should add `-DCMAKE_BUILD_TYPE=Debug` in the first CMake command.

## Examples

To use REmatch, you have two options. You can create a `Regex` object through the method `REMatch::compile` and pass the regular expression as argument, or you can directly call the functions provided. 

```cpp
// Compile a regular expression using the compile method and find a match
REMatch::Regex regex = REMatch::compile("!x{a}");
std::unique_ptr<REMatch::Match> regex_match = regex.find("aaa");

// Equivalent to calling find directly
std::unique_ptr<REMatch::Match> direct_match = REMatch::find("!x{a}", "aaa");
```

The `Regex` provides the methods `find` and `finditer` that evaluate a document and return the matches found. The `find` method returns a pointer to the first encountered match, while `finditer` returns an iterator that allows you to access all matches. To retrieve all the matches at once, you can use the `findall` method. You can use the `start` and `end` methods to obtain the indices of the matched spans or `span` to get a string representation.

### Retrieving all spans corresponding to a pattern

```cpp
#include <iostream>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "aaa";
  REMatch::Regex regex = REMatch::compile("!x{a}");
  REMatch::MatchIterator iterator = regex.finditer(document);
  std::unique_ptr<REMatch::Match> match = iterator.next();
  while (match != nullptr) {
    std::cout << "Span: [" << match->start("x") << ", " << match->end("x") << ">" << std::endl;
    match = iterator.next();
  }
  return 0;
}
```

### Finding the first match

```cpp
#include <iostream>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "aaa";
  REMatch::Regex regex = REMatch::compile("!x{a}");
  std::unique_ptr<REMatch::Match> match = regex.find(document);
  std::cout << "Span: " << match->span("x") << std::endl;
  return 0;
}
```

### Finding all matches

```cpp
#include <iostream>
#include <string>
#include "library_interface/rematch.hpp"

int main() {
  std::string document = "aaa";
  std::string regex = "!x{a}";
  std::vector<REMatch::Match> matches = REMatch::library_interface::findall(regex, document);
  for (REMatch::Match& match: matches) {
    std::cout << "Span: [" << match.start("x") << ", " << match.end("x") << ">" << std::endl;
  }
  return 0;
}
```

## Testing

We are using Catch2 for unit testing.

To add more tests, create files of the form: `tests/<module_name>/<class_tested>.cpp` and add these files to the TEST_SOURCES of `tests/CMakeLists.txt`.

## Automatic documentation

to build the automatic documentation two packages are needed: graphviz and doxygen. To build the documentation run `doxygen DoxyFile`.
