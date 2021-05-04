#ifndef PARSER_AUTOMATA_HPP
#define PARSER_AUTOMATA_HPP

#include "automata/nfa/lva.hpp"

rematch::LogicalVA& parse_automata_file(std::string filename);

#endif