# Implementation of constant delay algorithm for regular document spanners

This implementation is based on the paper `Constant delay algorithms for regular document spanners` by Fernando Florenzano, Cristian Riveros, Martín Ugarte, Stijn Vansummeren and Domagoj Vrgoč.

### Directory structure

The C++ implementation is under `/src` folder.

The `/exp` folder contains different experiments to compare our library with others.

The `/tests` folder contains all the automatic tests for our code.

### Build instructions

#### Python/SWIG

Assuming that you are in a Debian-based distro, first install the following dependencies:
```
sudo apt install g++ cmake swig libboost-dev python3-dev
```
After that, in this directory, run:
```
mkdir -pv build && cd build
cmake -DSWIG=true ..
make
```
After the compilation process there will be a `rematch.py` (the bindings interface) and a `_rematchswiglib.so` (the shared lib binary) in `build/bin/SWIG` that you can use for interfacing REmatch via Python.

#### CLI tool

```
cmake -H. -Bbuild/Release
cmake --build build/Release
```

If you want to use a debugger such as `gdb`, then you should add `-DCMAKE_BUILD_TYPE=Debug` in the first CMake command.

### Command line use

After building, the binary file will be located in the `build/Release/bin` folder. To try it simply run:

```
build/Release/bin/rematch --help
```

#### Examples:

Get all spans corresponding to a single letter `a`:
```
build/Release/bin/rematch -d document.txt -e '.*!x{a}.*'
```
Get all spans corresponding to a pattern in a file:
```
build/Release/bin/rematch -d document.txt -r regex.txt
```
Get benchmark stats (execution time, number of outputs, memory usage, etc.):
```
build/Release/bin/rematch -d document.txt -r regex.txt -o benchmark
```


### Testing

We are using Boost.Test for unit testing.

To add more tests, add a new folder inside `tests/[test_name_folder]/` that starts with the word _test_ as a
prefix. Follow the same structure (same file names) of the other folders.
