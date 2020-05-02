# Implementation of constant delay algorithm for regular document spanners

This implementation is based on the paper `Constant delay algorithms for regular document spanners` by Fernando Florenzano, Cristian Riveros, Martín Ugarte, Stijn Vansummeren and Domagoj Vrgoč.

### Directory structure

The C++ implementation is under `/src` folder.

The `/exp` folder contains different experiments to compare our library with others.

The `/tests` folder contains all the automatic tests for our code.

### Build instructions

```
cd src
mkdir build
cd build
cmake .. 
make
```

If you want to use a debugger such as `gdb`, then you should add `-DCMAKE_BUILD_TYPE=Debug` in the CMake command.

### Command line use

After building, the binary file will be located in the `src/build/` folder. To try it simply run:

```
./rematch --help
```

### Testing

We are using Boost.Test for unit testing.

To add more tests, add a new folder inside `tests/[test_name_folder]/` that starts with the word _test_ as a 
prefix. Follow the same structure (same file names) of the other folders.
