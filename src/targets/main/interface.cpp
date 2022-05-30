#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

#include <sys/resource.h>

#include "automata/dfa/dfa.hpp"
#include "automata/nfa/eva.hpp"
#include "evaluation/evaluator.hpp"
#include "interface.hpp"
#include "matchiterator.hpp"
#include "memmanager.hpp"
#include "parse/parser.hpp"
#include "regex/regex.hpp"
#include "regex/regex_options.hpp"
#include "util/timer.hpp"

Interface::Interface(std::string &docstr, const std::string &pattern,
                     rematch::Options opt)
    : document_(nullptr), pattern_(pattern), options_(opt) {
  if (opt.is_docfile()) {
    document_ = std::make_shared<rematch::FileDocument>(docstr);
  } else {
    document_ = std::make_shared<rematch::StrDocument>(docstr);
  }
}

void Interface::run() {
  if (options_.output_option() == rematch::BENCHMARK ||
      options_.output_option() == rematch::DEBUG) {
    benchmark_run();
  } else {
    normal_run();
  }
}

void Interface::normal_run() {
  rematch::RegExOptions rgx_opts; // Assign options for regex

  // Select options
  rgx_opts.set_line_by_line(options_.line_by_line());
  rgx_opts.set_early_output(options_.early_output());
  rgx_opts.set_searching(options_.searching());
  rgx_opts.set_macrodfa(options_.macrodfa());

  rematch::RegEx regex(pattern_, rgx_opts);

  rematch::MatchIterator m_iter = regex.findIter(document_);

  auto out_option = options_.output_option();

  if (out_option == rematch::SPANS) {
    for (auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
      std::cout << *match << std::endl;
    }
  } else if (out_option == rematch::NMAPPINGS) {
    size_t count = 0;
    for (auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
      count++;
    }
    std::cout << count << std::endl;
  } else if (out_option == rematch::SUBMATCHES) {
    auto str_doc = std::static_pointer_cast<rematch::StrDocument>(document_);
    for (auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
      std::cout << *match << "\t(" << match->pprint(str_doc) << ")"
                << std::endl;
    }
  }
}

void Interface::benchmark_run() {
  std::stringstream output;

  size_t n_mappings, detSize, nfaSize, mdfaSize, n_segments;
  double initAutomataTime, evaluateTime, totTime;
  /**************************** Run Algorithm ****************************/

  Timer t; // Start timer for automata creation

  rematch::RegExOptions rgx_opt;
  rgx_opt.set_line_by_line(options_.line_by_line());
  rgx_opt.set_early_output(options_.early_output());
  rgx_opt.set_searching(options_.searching());
  rgx_opt.set_macrodfa(options_.macrodfa());
  rematch::RegEx regex(pattern_, rgx_opt);
  rematch::Match_ptr match_ptr;

  initAutomataTime = t.elapsed(); // Automata creation time
  t.reset();                      // Start timer for evaluation time

  n_mappings = 0;

  rematch::MatchIterator match_iter = regex.findIter(document_);

  for (auto match = match_iter.next(); match != nullptr;
       match = match_iter.next()) {
    n_mappings++;
  }

  nfaSize = match_iter.stats_->eva_size;
  n_segments = match_iter.stats_->n_search_intervals;

  // numOfCaptures = regex.capture_counter();
  // numOfReadings = regex.reading_counter();

  detSize = match_iter.stats_->dfa_size;
  mdfaSize = match_iter.stats_->mdfa_size;

  std::ofstream mfile("dump.log");

  for (auto &e : match_iter.stats_->search_intervals) {
    mfile << "Segment: |" << e.first << ',' << e.second << ">\n";
  }

  mfile.close();

  // std::cout << "\nRaw DFA:\n" <<  regex.detManager().DFA().pprint() << '\n';
  // std::cout << "\nRaw NFA:\n" <<  regex.detManager().NFA().pprint() << '\n';

  evaluateTime = t.elapsed(); // Evaluation time

  /************************** Get Measurments **************************/

  totTime = initAutomataTime + evaluateTime;

  // GET MEMORY USAGE
  struct rusage usage;
  int ret;
  std::string memoryUsed;
  ret = getrusage(RUSAGE_SELF, &usage);
  if (ret == 0) {
    memoryUsed = formatMem(usage.ru_maxrss * 1024);
  }

  /************************ Output Measurments ************************/

  std::cout << "Number of mappings\t\t" << pwc(n_mappings) << '\n'
            << "Memory used \t\t\t" << memoryUsed << '\n'
            << "MDFASize \t\t\t" << mdfaSize << '\n'
            << "DetSize \t\t\t" << detSize << '\n'
            << "eVASize \t\t\t" << nfaSize << '\n'
            << "Number of segments\t\t" << n_segments << '\n'
            << "Init Automata time\t\t" << pwc(initAutomataTime) << " ms\n"
            << "Evaluate time\t\t\t" << pwc(evaluateTime) << " ms\n"
            << "Total time\t\t\t" << pwc(totTime) << " ms\n\n";

#ifdef COUNT_CURRENT_STATES
  auto max_pair = match_iter.evaluator_->max_count_states();
  std::cout << "Max state count:\n"
            << max_pair.first << "\t" << max_pair.second << "\n";
#endif
}

std::string Interface::formatMem(size_t sizeInBytes) {
  /**
   * Format size in bytes to readable form in Kb, Mb or Gb
   */

  std::stringstream ss;
  double sizeForm = (double)sizeInBytes;
  std::string units[4] = {"", "K", "M", "G"};
  int counter = 0;

  while (sizeForm > 1024) {
    sizeForm /= 1024;
    counter++;
  }

  ss << sizeForm << units[counter];

  return ss.str();
}

std::string Interface::pwc(size_t value) {
  std::string numWithCommas = std::to_string(value);
  int insertPosition = numWithCommas.length() - 3;
  while (insertPosition > 0) {
    numWithCommas.insert(insertPosition, ",");
    insertPosition -= 3;
  }
  return numWithCommas;
}