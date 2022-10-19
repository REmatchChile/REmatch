#include <iostream>
#include <string>

#include <CLI11.hpp>

#include "interface.hpp"
#include "options.hpp"
#include "util/util.hpp"

int main(int argc, char *argv[]) {

  std::string doc, rgx;
  rematch::Options opts;

  CLI::App app{"REmatch"};

  auto docinput = app.add_option_group("document_input");
  auto rgxinput = app.add_option_group("regex_input");

  std::string docpath, rgxpath, mode{"spans"}, anchors{"none"};

  // Document input options
  docinput
      ->add_option("-d,--document,document", docpath, "Input document file.")
      ->check(CLI::ExistingFile);
  docinput->add_option("-t,--text", doc, "Input text.");
  docinput->require_option(1);

  // Regex input optsions
  rgxinput
      ->add_option("-r,--regex-file,regex-file", rgxpath, "Input regex file.")
      ->check(CLI::ExistingFile);
  rgxinput->add_option("-e,--expression", rgx, "Input regular expression.");
  rgxinput->require_option(1);

  // Output mode
  app.add_option("--mode", mode, "Mode option.")
      ->check(CLI::IsMember({"benchmark",
                             "noutputs",
                             "spans",
                             "submatches",
                             "ambiguous"}));

  // // Anchor specification
  // app.add_option("-a,--anchors",anchors,"Use anchors for regex")
  // 	->check(CLI::IsMember({"both", "left", "right", "none"}));

  bool cont{false}, line{false}, searching{false}, macrodfa{false},
       unambiguous{false};

  // Evaluation flags
  app.add_flag("-c,--continuous", cont, "Continuous evaluation.");
  app.add_flag("-l,--line-by-line", line, "Line by line evaluation");
  app.add_flag("--searching", searching, "Searching phase activation.");
  app.add_flag("--macrodfa", macrodfa, "MacroDFA usage activation.");
  app.add_flag("--unambiguous", unambiguous, "Unambiguous searching");

  CLI11_PARSE(app, argc, argv);

  opts.set_early_output(cont);
  opts.set_line_by_line(line);
  opts.set_searching(searching);
  opts.set_macrodfa(macrodfa);
  opts.set_unambiguous(unambiguous);

  if (mode == "benchmark")
    opts.set_output_option(rematch::BENCHMARK);
  else if (mode == "debug")
    opts.set_output_option(rematch::DEBUG);
  else if (mode == "noutputs")
    opts.set_output_option(rematch::NMAPPINGS);
  else if (mode == "submatches")
    opts.set_output_option(rematch::SUBMATCHES);
  else if (mode == "ambiguous")
    opts.set_output_option(rematch::AMBIGUOUS);

  if (rgxpath.size())
    rgx = rematch::util::file2str(rgxpath);

#ifndef BENCHMARK_MODE
  if (docpath.size()) {
    opts.set_is_docfile(true);
    doc = docpath;
  }
#else
  if (docpath.size()) {
    doc = rematch::util::file2str(docpath);
  }
#endif

  Interface intf(doc, rgx, opts);

  intf.run();

  return 0;
}