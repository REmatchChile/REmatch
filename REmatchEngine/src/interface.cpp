#include <sstream>
#include <istream>
#include <iostream>
#include <fstream>

#ifdef linux
#include <sys/resource.h>
#endif

#include "interface.hpp"
#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "memmanager.hpp"
#include "parser_automata/parser.hpp"
#include "timer.hpp"
#include "regex/regex.hpp"
#include "regex/regex_options.hpp"
#include "eval.hpp"

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
	if(!options_.line_by_line()) {
		std::string doc(file2str(document_filename_));
		if(options_.output_option() == NMAPPINGS) {
			size_t noutputs = 0;
			auto finditer = regex.findIter(doc);
			while((match_ptr = finditer.next())) {
				noutputs++;
			}
			std::cout << noutputs << '\n';
		}
		else {
			auto finditer = regex.findIter(doc);
			while((match_ptr = finditer.next())) {
				std::cout << *match_ptr << '\n';
			}
		}
	} else {
		if(options_.output_option() == NMAPPINGS) {
			size_t noutputs = 0;
			auto finditer = regex.findIterFile(*document_stream_);
			while((match_ptr = finditer.next())) {
				noutputs++;
			}
			std::cout << noutputs << '\n';
		}
		else {
			auto finditer = regex.findIterFile(*document_stream_);
			while((match_ptr = finditer.next())) {
				std::cout << *match_ptr << '\n';
			}
		}
	}
}

void Interface::benchmarkRun() {
    std::stringstream output;

	size_t numOfSpans, numOfCaptures, numOfReadings;
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

	auto finditer = regex.findIterFile(*document_stream_);
	while((match_ptr = finditer.next())) {
		numOfSpans++;
	}

	numOfCaptures = 0;
	numOfReadings = 0;

	evaluateTime = t.elapsed(); 			// Evaluation time

	/************************** Get Measurments **************************/

	totTime = initAutomataTime + evaluateTime;

	std::string memoryUsed;

	#ifdef linux
	// GET MEMORY USAGE
	struct rusage usage;
	int ret;
	
	ret = getrusage(RUSAGE_SELF, &usage);
	if(ret == 0) {
		memoryUsed = formatMem(usage.ru_maxrss*1024);
	}
	#endif



	/************************ Output Measurments ************************/

	std::cout
				<< "Number of mappings\t\t" 		<< 	numOfSpans				<<	'\n'
				<< "Memory used \t\t\t"					<<	memoryUsed	 			<< 	'\n'
				<< "Num of Captures\t\t\t"			<< 	numOfCaptures			<<	'\n'
				<< "Num of Readings\t\t\t"			<< 	numOfReadings			<<	'\n'
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
