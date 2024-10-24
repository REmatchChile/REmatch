<img alt="REmatch" style="height:48px;" src="https://raw.githubusercontent.com/REmatchChile/REmatch-web/refs/heads/main/src/assets/logo-dark.png"/>

---

Here you can find the main implementation of REmatch library in C++, and its bindings for both Python and JavaScript/WebAssembly. This version has been refactorized, tested, and developed for being ready for production. Don't forget to visit our website [rematch.cl](https://rematch.cl) and the [official REmatch Wiki](https://github.com/REmatchChile/REmatch/wiki)!

Check out our library in different languages!

- [rematch-javascript on NPM](https://www.npmjs.com/package/rematch-javascript)
- [pyrematch on PyPI](https://pypi.org/project/pyrematch/)

## Table of contents

1. [Directory structure](#1-directory-structure)
2. [Build instructions](#2-build-instructions)
3. [Examples](#3-examples)
4. [Building JavaScript/WASM bindings](#4-building-javascriptwasm-bindings)
5. [Building Python bindings](#5-building-python-bindings)

## 1. Directory structure

```txt
📦REmach
├── 📂cmake/ ---------------------------- Multiple utilities for CMake
├── 📂datasets/ ------------------------- Contains all datasets used for testing
├── 📂python/ --------------------------- Python package interface for the bindings
├── 📂scripts/ -------------------------- Contains useful scripts for testing and profiling
├── 📂src/ ------------------------------ The C++ implementation.
├── 📂test/ ----------------------------- Contains all tests for the code.
├── 📂third_party/ ---------------------- Third-party libraries used in the project
├── 📜CMakeLists.txt
├── 📜LICENSE
├── 📜MANIFEST.in
├── 📜README.md
├── 📜pyproject.toml
└── 📜setup.py
```

## 2. Build instructions

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

### Installation

Build REmatch in `Release` mode with one of the following command:

```bash
# Unix
cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release/

# Windows
cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release/ --config Release
```

After building, the library and binaries will be located in the `./build/Release/` folder. To install the library in your system, run the following command:

```bash
cmake --install build/Release/
```

### Usage

After successfully installing the project, you can start using the library in your C++ programs.

#### Quick usage with CMake

Inside your project directory, lest say `hello-rematch`:

Create `hello-rematch/main.cpp`:

```cpp
#include <iostream>
#include <REmatch/REmatch.hpp>

int main() {
  auto query = REmatch::reql("!x{that}");
  auto match_generator = query.finditer("thathathat");

  for (auto match : match_generator) {
    std::cout << match << "\n";
  }

  return 0;
}
```

Create `hello-rematch/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(hello-rematch)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(REmatch REQUIRED)

add_executable(main main.cc)
target_link_libraries(main REmatch::REmatch)
```

Finally, compile your program using the following command:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Quick usage without build system (UNIX)

Assuming that you have the file `hello-rematch/main.cpp` as the previous section, you can compile it with the following command:

```bash
# gcc
g++ main.cpp -std=c++17 -lREmatch -o main

# clang
clang++ main.cpp -std=c++17 -lREmatch -o main
```

Depending on your system configuration you may need to add the include and library paths to the compilation command.

### Debugging

If you want to use a debugger such as `gdb`, then you should run the following command:

```bash
# Unix
cmake -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug/

# Windows
cmake -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug/ --config Debug
```

In this case, the build path will be `build/Debug/bin`.

## Testing

We are using Catch2 for unit testing. To add more tests, create files at: `tests/<module_name>/<class_tested>.cpp`.

To build REmatch testing executable, turn the `BUILD_TESTING` variable `ON` at `CMakeLists.txt`, and rebuild the project. Then you can run:

```bash
./build/Release/bin/rematch_test
```

### Profiler

We are using the Tracy profiler to measure the time spent in specific code segments. To profile the code using the graphical user interface, follow these steps:

1. Turn the `ENABLE_PROFILING` variable `ON` at `CMakeLists.txt`, and rebuild the project.

2. Navigate to `build/Release/_deps/tracy-src/profiler/build/unix` and run `make` to generate the "Tracy-release" executable.

3. Execute the "Tracy-release" executable.

4. Run REmatch to initiate the profiling.

You should be able to view the results in the graphical interface.

## 3. Examples

To use REmatch, you must use the entry-point function `reql` as follows:

```cpp
// Compile a regular expression and find a single match
REmatch::Query query = REmatch::reql("!x{aba}");

REmatch::Match match = query.findone("aabaa");
```

The `Query` provides the methods `findone`, `findmany`, `findall` and `finditer` that evaluate a document and return the matches found. The `findone` method returns a pointer to the first encountered match, while `finditer` returns an iterator that allows you to access all matches. To retrieve all the matches at once, you can use the `findall` method (or up to a limit with `findmany`). You can use the `start` and `end` methods to obtain the indices of the matched spans or `span` to get a pair representation.

Notice that `finditer` generates the matches on the fly, so if you want to minimize the memory overhead while processing each match you should prefer this method over `findall`.

### Retrieving an iterator for the pattern aba

```cpp
#include <memory>
#include <string>

#include <REmatch/REmatch.hpp>

int main() {
  std::string document = "abaababa";
  REmatch::Query query = REmatch::reql("!x{aba}");
  REmatch::MatchGenerator match_generator = query.finditer(document);
  for (auto& match : match_generator) {
    std::cout << "Match: " << match << std::endl;
  }
  return 0;
}
```

### Finding the first match for the pattern aba

```cpp
#include <iostream>
#include <string>

#include <REmatch/REmatch.hpp>

int main() {
  std::string document = "abaababa";
  REmatch::Query query = REmatch::reql("!x{aba}");
  REmatch::Match match = query.findone(document);
  std::cout << "Match: " << match << std::endl;

  return 0;
}
```

### Finding a vector with all matches for the pattern aba

```cpp
#include <iostream>
#include <string>

#include <REmatch/REmatch.hpp>

int main() {
  std::string document = "aba";
  REmatch::Query query = REmatch::reql("!x{aba}");
  std::vector<REmatch::Match> matches = query.findall(document);
  for (auto& match : matches) {
    std::cout << "Match: " << match << std::endl;
  }
  return 0;
}
```

Also we have an advanced interface for "MultiQuerying". You can read more about this in the [REmatch's wiki](https://github.com/REmatchChile/REmatch/wiki).

### 4. Building JavaScript/WASM bindings

To begin, follow the official installation tutorial for Emscripten available at <https://emscripten.org/docs/getting_started/downloads.html>

Once the installation is complete, execute the following command:

```bash
emcmake cmake -Bbuild/javascript -DCMAKE_BUILD_TYPE=Release
cmake --build build/javascript/
```

Then the JavaScript/WASM bindings can be found in `./build/javascript/bin/`. Notice that it is not intended to use this bindings directly. Prefer using the [REmatch's NPM package](https://www.npmjs.com/package/rematch-javascript).

### 5. Building Python bindings

To install the latest release version from PyPI, run:

```bash
pip install pyrematch
```

To build from the source code, clone this repository and run:

```bash
pip install .
```
