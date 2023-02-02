# REmatch: An information extraction tool for regular document spanners

This implementation is based on the paper `REmatch: a regex engine
for finding all matches` by Cristian Riveros, Nicolar Van Sint Jan and
Domagoj Vrgoč.

## Directory structure

The C++ implementation is under `/src` folder.

The `/tests` folder contains all the automatic tests for our code.

The `/scripts` folder contains multiple useful scripts for setting up the project.

### Build instructions

The setup of CMake, Catch2, boost and g++-11 on a clean ubuntu system can be done using the script `setup/ubuntu/setup_cpp_to_execute_rematch.sh`

Other scripts are self explanatory.

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

We are using Catch2 for unit testing.

To add more tests, create files of the form:
`tests/<module_name>/<class_tested>.cpp` and add these files to the
TEST_SOURCES of `tests/CMakeLists.txt`


## Automatic documentation

to build the automatic documentation two packages are needed: graphviz and doxygen. To build the documentation run `doxygen DoxyFile`
