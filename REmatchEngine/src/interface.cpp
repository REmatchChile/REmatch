#include <sstream>
#include <istream>
#include <iostream>
#include <fstream>

#include <sys/resource.h>

#include "interface.hpp"
#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "memmanager.hpp"
#include "parser_automata/parser.hpp"
#include "timer.hpp"
#include "regex/regex.hpp"
#include "regex/regex_options.hpp"

using namespace rematch;

Interface::Interface(std::string docFile, std::string input,
					 Options opt) :

    document_stream_(new std::ifstream(docFile)),
    document_filename_(docFile),
	options_(opt) {

	switch (options_.input_option()) {
	case RGXSTR:
		pattern_ = input;
		break;
	case RGXFILE:
		pattern_ = file2str(input);
		break;
	case NFAFILE:
	// 	lv_automaton_ = parse_automata_file(input);
	// 	aux_automaton_ = parse_automata_file(input);
		break;
	}
}

void Interface::run() {
    if(options_.output_option() == BENCHMARK || options_.output_option() == DEBUG) {
        benchmarkRun();
    } else {
        normalRun();
    }
}

void Interface::normalRun() {
	RegExOptions rgx_opt;
	rgx_opt.set_line_by_line(options_.line_by_line());
	RegEx regex(pattern_, rgx_opt);
	Match_ptr match_ptr;
	std::string doc(file2str(document_filename_));
	if(options_.output_option() == NMAPPINGS) {
		size_t noutputs = 0;
		while((match_ptr = regex.findIter(*document_stream_))) {
			noutputs++;
		}
		std::cout << noutputs << '\n';
	}
	else {
		while((match_ptr = regex.findIter(*document_stream_))) {
			std::cout << *match_ptr << '\n';
		}

		// std::cout << "DFA: \n" << regex.detManager().DFA().pprint() << "\n\n";
	}
}

void Interface::benchmarkRun() {
    std::stringstream output;

	size_t eVASize, detASize, numOfSpans,
		numOfNodes, numOfNodeArenas,
		numOfNodesReused;
	double initAutomataTime, evaluateTime, totTime;
	/**************************** Run Algorithm ****************************/

	Timer t; 								// Start timer for automata creation

	RegExOptions rgx_opt;
	rgx_opt.set_line_by_line(options_.line_by_line());
	RegEx regex(pattern_, rgx_opt);
	Match_ptr match_ptr;

	initAutomataTime = t.elapsed(); 		// Automata creation time
	t.reset(); 								// Start timer for evaluation time

	numOfSpans = 0;

	while((match_ptr = regex.findIter(*document_stream_))) {
		numOfSpans++;
	}

	evaluateTime = t.elapsed(); 			// Evaluation time

	/************************** Get Measurments **************************/

	totTime = initAutomataTime + evaluateTime;

	eVASize = 0;
	detASize = 0;

	// GET MEMORY USAGE
	struct rusage usage;
	int ret;
	std::string memoryUsed;
	ret = getrusage(RUSAGE_SELF, &usage);
	if(ret == 0) {
		memoryUsed = formatMem(usage.ru_maxrss*1024);
	}

	/************************ Output Measurments ************************/

	std::cout
				<< "ExtendedVA size\t\t\t"			<< 	eVASize						<< 	'\n'
				<< "DetVA size\t\t\t" 					<< 	detASize					<<	'\n'
				<< "Number of mappings\t\t" 		<< 	numOfSpans				<<	'\n'
				<< "Number of nodes\t\t\t"			<<	numOfNodes 				<< 	'\n'
				<< "Number of node arenas\t\t"	<<	numOfNodeArenas 	<< 	'\n'
				<< "Memory used \t\t\t"					<<	memoryUsed	 			<< 	'\n'
				<< "Nodes reused\t\t\t"					<<	numOfNodesReused 	<< 	'\n'
				<< "Init Automata time\t\t"			<<	initAutomataTime 	<< 	"s\n"
				<< "Evaluate time\t\t\t"				<<	evaluateTime			<< 	"s\n"
				<< "Total time\t\t\t"						<<	totTime 					<< 	"s\n";

}

std::string Interface::formatMem(size_t sizeInBytes) {
    /**
	 * Format size in bytes to readable form in Kb, Mb or Gb
	 */

	std::stringstream ss;
	double sizeForm = (double)sizeInBytes;
	std::string units[4] = {"", "K", "M", "G"};
	int counter = 0;

	while(sizeForm > 1024) {
		sizeForm /= 1024;
		counter++;
	}

	ss << sizeForm << units[counter];

	return ss.str();
}

std::string Interface::file2str(std::string filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, in.end);
		contents.resize(in.tellg());
		in.seekg(0, in.beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw std::runtime_error("Error loading file");
}
