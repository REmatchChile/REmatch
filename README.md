# REmatch: An information extraction tool for regular document spanners 

This implementation is based on the paper `Constant delay algorithms for regular document spanners` by Fernando Florenzano, Cristian Riveros, Martín Ugarte, Stijn Vansummeren and Domagoj Vrgoč.

### Directory structure

The C++ implementation is under `/src` folder.

The `/exp` folder contains different experiments to compare our library with others.

The `/tests` folder contains all the automatic tests for our code.

### Build instructions

The setup of CMake, Catch2, boost and g++-11 on a clean ubuntu system can be done using the script `setup_cpp_to_execute_rematch.sh`
Use the script "run_and_execute.sh" to build and run the tests.

If you want to use a debugger such as `gdb`, then you should add `-DCMAKE_BUILD_TYPE=Debug` in the first CMake command.

### Command line use

After building, the binary file will be located in the `build/Release/bin` folder. To try it simply run:

```
build/Release/bin/rematch --help
```

#### Examples:

Get all spans corresponding to a single letter `a`:
```
build/Release/bin/rematch document.txt -e '.*!x{a}.*'
```
Same as above but giving the text explicitly from args:
```
build/Release/bin/rematch -t "bbbaaabbb" -e '.*!x{a}.*'
```
Get all spans corresponding to a pattern in a file:
```
build/Release/bin/rematch document.txt regex.txt
```
Get benchmark stats (execution time, number of outputs, memory usage, etc.):
```
build/Release/bin/rematch document.txt regex.txt --mode=benchmark
```


### Testing

We are using Boost.Test for unit testing.

To add more tests, add a new folder inside `tests/[test_name_folder]/` that starts with the word _test_ as a
prefix. Follow the same structure (same file names) of the other folders.


## Automatic documentation

to build the automatic documentation two packages are needed:

graphviz and doxygen.
