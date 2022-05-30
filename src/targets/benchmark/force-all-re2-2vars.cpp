#include <iostream>

#include <re2/re2.h>

#include "util/util.hpp"

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0]
              << " [rgx_file] [doc_file]\n";
    exit(1);
  }

  std::string doc, rgx1;

  rgx1 = file2str(argv[2]);
  rgx2 = file2str(argv[3]);

  doc = file2str(argv[1]);

  re2::StringPiece input(doc);

  RE2 pattern(rgx1);
  RE2 subpattern(rgx2);

  re2::StringPiece supermatch, match, super_submatch, submatch;
  int count = 0;
  size_t inp_sz = input.size(); // Original size

  for (size_t i = 0; i < inp_sz; i++) {
    if (RE2::PartialMatch(input, pattern, &supermatch)) {
      size_t spmatch_size = supermatch.size();
      for (size_t j = 0; j < spmatch_size; j++) {
        if (RE2::PartialMatch(supermatch, subpattern, &match)) {
          size_t match_size = match.size();
          for (size_t i = 0; i < match_size; i++) {
            if (RE2::FullMatch(match, subpattern, &super_submatch)) {
              size_t sp_submatch_size = super_submatch.size();
              for (size_t j = 0; j < sp_submatch_size; j++) {
                if (RE2::PartialMatch(super_submatch, subpattern, &submatch)) {
                  ++count;
                  std::cout << "|" << submatch.data() - doc.data() << ","
                            << submatch.data() - doc.data() + submatch.size()
                            << ">\n";
                } else
                  break;
                // No se puede asegurar que no haya output en el resto de los
                // prefijos.
                super_submatch.remove_suffix(super_submatch.size() -
                                             submatch.size() + 1);
              }
              // std::cout << "Match: \"" << match << "\"\n";

              // Jump to the start of the next match
              match.remove_prefix(1);
            } else
              break;
          }
          // std::cout << "|" << match.data() - doc.data() << ","
          //                  << match.data() - doc.data() + match.size() <<
          //                  ">\n";
        } else
          break;
        // No se puede asegurar que no haya output en el resto de los
        // prefijos.
        supermatch.remove_suffix(supermatch.size() - match.size() + 1);
      }
      // std::cout << "Match: \"" << match << "\"\n";

      // Jump to the start of the next match
      input.remove_prefix(supermatch.data() - input.data() + 1);
    } else
      break;
  }

  std::cout << count << '\n';
  return 0;
}