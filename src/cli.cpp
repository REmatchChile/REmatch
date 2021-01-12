#include <iostream>
#include <cxxopts.hpp>

#include "cli.hpp"
#include "interface.hpp"
#include "options.hpp"

using namespace rematch;

cxxopts::ParseResult parse(int argc, char *argv[]) {

	std::string documentFile, regex, regexFile, automataFile, input;
	Options Opt;

	try {

		cxxopts::Options options{"Options"};
		options.add_options()
			("h,help", "Help screen")
			("o,output", "Output option", cxxopts::value<std::string>()->default_value("spans"))
			("e,expression", "Regular expression", cxxopts::value(regex))
			("r,rfile", "Regex file", cxxopts::value(regexFile))
			("a,afile", "Automata file", cxxopts::value(automataFile))
			("d,doc", "Document file", cxxopts::value(documentFile))
			("c,continuous", "Enable continuous output")
			("l,linebyline", "Enable line by line evaluation");

		options.parse_positional({"doc", "expression"});

		auto result = options.parse(argc, argv);

		if(result.count("help")) {
			std::cout << "Syntax:\n\t "<< argv[0] <<
						 " [OPTION]... [document-file] [expression]\n" << options.help() << '\n';
			exit(0);
		}

		int exprInput = result.count("expression") + result.count("afile") + result.count("rfile");
		if(!result.count("doc") || exprInput != 1) {
			std::cerr << "Error at parsing parameters."
			          << " Type\n\t" << argv[0] << " --help\n" << "to show syntax and available options.\n";
			exit(1);
		}


		Opt.set_early_output((bool)result.count("continuous"));
		Opt.set_line_by_line((bool)result.count("linebyline"));

		if(result.count("output")) {
			std::string outputOpt = result["output"].as<std::string>();
			if(outputOpt == "spans") {
				Opt.set_output_option(SPANS);
			} else if(outputOpt == "benchmark") {
				Opt.set_output_option(BENCHMARK);
			} else if(outputOpt == "quiet") {
				Opt.set_output_option(QUIET);
			} else if(outputOpt == "time") {
				Opt.set_output_option(TIME);
			} else if(outputOpt == "nmappings") {
				Opt.set_output_option(NMAPPINGS);
			} else if(outputOpt == "debug") {
				Opt.set_output_option(DEBUG);
			}
			else {
				std::cerr << "Error: --output=" << outputOpt << " option not understood\n";
			}
		}

		if (result.count("rfile")) {
			input = regexFile;
			Opt.set_input_option(RGXFILE);
		} else if (result.count("afile")) {
			input = automataFile;
			Opt.set_input_option(NFAFILE);
		} else if (result.count("expression")) {
			input = regex;
			Opt.set_input_option(RGXSTR);
		}

		Interface interface(documentFile, input, Opt);

		interface.run();

		return result;
	}
	catch (const cxxopts::OptionException& e) {
		std::cerr << "Error parsing options: " << e.what() << '\n';
		exit(1);
	}

}