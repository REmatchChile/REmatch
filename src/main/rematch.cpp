#include <iostream>
#include <string>

#include "CLI11.hpp"

#include "options.hpp"
#include "interface.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {

	std::string doc, rgx;
	rematch::Options opts;

	CLI::App app{"REmatch"};

	auto docinput = app.add_option_group("document_input");
	auto rgxinput = app.add_option_group("regex_input");

	std::string docpath, rgxpath, mode{"spans"}, anchors{"none"};

	// Document input options
	docinput->add_option("-d,--document,document", docpath, "Input document file.")
			 																								->check(CLI::ExistingFile);
	docinput->add_option("-t,--text", doc, "Input text.");
	docinput->require_option(1);

	// Regex input optsions
	rgxinput->add_option("-r,--regex-file,regex-file",rgxpath,"Input regex file.")
																										 ->check(CLI::ExistingFile);
	rgxinput->add_option("-e,--expression", rgx, "Input regular expression.");
	rgxinput->require_option(1);

	// Output mode
	app.add_option("--mode",mode,"Mode option.")
		->check(CLI::IsMember({"benchmark","noutputs","spans","submatches"}));

	// // Anchor specification
	// app.add_option("-a,--anchors",anchors,"Use anchors for regex")
	// 	->check(CLI::IsMember({"both", "left", "right", "none"}));

	bool cont{false}, line{true};

	// Evaluation flags
	// app.add_flag("-c,--continuous",cont,"Continuous evaluation.");
	app.add_flag("-l,--line-by-line", line,"Line by line evaluation");

	CLI11_PARSE(app, argc, argv);

	opts.set_early_output(cont);
	opts.set_line_by_line(line);

	if(mode == "benchmark")
		opts.set_output_option(rematch::BENCHMARK);
	else if(mode == "debug")
		opts.set_output_option(rematch::DEBUG);
	else if(mode == "noutputs")
		opts.set_output_option(rematch::NMAPPINGS);
	else if(mode == "submatches")
		opts.set_output_option(rematch::SUBMATCHES);

	if(rgxpath.size())
		rgx = file2str(rgxpath);

	if(docpath.size() && line) {
		opts.set_is_docfile(true);
		doc = docpath;
	} else if(docpath.size()) {
		doc = file2str(docpath);
	}

	Interface intf(doc, rgx, opts);

	intf.run();

	return 0;
}