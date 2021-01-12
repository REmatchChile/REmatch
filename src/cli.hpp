#ifndef CLI_H
#define CLI_H

#include <string>
#include <cxxopts.hpp>


cxxopts::ParseResult parse(int argc, char *argv[]);

#endif