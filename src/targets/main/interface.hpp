#ifndef INTERFACE_H
#define INTERFACE_H

#include <fstream>
#include <memory>
#include <string>

#include "automata/nfa/lva.hpp"
#include "evaluation/document/document.hpp"
#include "options.hpp"

class Interface {
 public:
  Interface(const std::string& doc, const std::string& pattern,
            rematch::Options opt);

  // Algorithm runs
  void run();

 private:
  void normal_run();
  void benchmark_run();

  // Utilities
  std::string formatMem(size_t sizeInBytes);

  // Prints value with commas (e.g. 34,523,003)
  std::string pwc(size_t value);

  // Docstring might be a path to a file to-be-read or the document per se.
  // This is indicated by options_.is_docfile() .
  const std::string& document_;

  // Regular pattern given
  const std::string& pattern_;

  // Options for commanding evaluation
  rematch::Options options_;
};

#endif
