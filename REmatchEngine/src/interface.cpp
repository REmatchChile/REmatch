#include <sstream>
#include <istream>
#include <iostream>
#include <fstream>

#include <sys/resource.h>

#include "interface.hpp"
#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "evaluation.hpp"
#include "automata/detautomaton.hpp"
#include "memmanager.hpp"
#include "parser_automata/parser.hpp"
#include "timer.hpp"

using namespace rematch;

Interface::Interface(std::string docFile, std::string input,
					 Options opt) :

    document_stream_(new std::ifstream(docFile)),
    document_filename_(docFile),
	options_(opt) {

	switch (options_.input_option()) {
	case RGXSTR:
		lv_automaton_ = regex2LVA(input);
		aux_automaton_ = regex2LVA(input);
		break;
	case RGXFILE:
		lv_automaton_ = regex2LVA(file2str(input));
		aux_automaton_ = regex2LVA(file2str(input));
		break;
	case NFAFILE:
		lv_automaton_ = parse_automata_file(input);
		aux_automaton_ = parse_automata_file(input);
		break;
	}
	aux_automaton_.adapt_capture_jumping();
}

void Interface::run() {
    if(options_.output_option() == BENCHMARK || options_.output_option() == DEBUG) {
        benchmarkRun();
    } else {
        normalRun();
    }
}

void Interface::normalRun() {
	// LogicalVA A(lv_automaton_);
	// LogicalVA C = lv_automaton_;
	ExtendedVA B(lv_automaton_);
	ExtendedVA R(aux_automaton_);
	if(options_.output_option() == QUIET) {
		std::stringstream s;
		Evaluation eval(&B, &R, *document_stream_, s,
						options_.early_output(), options_.line_by_line());
		eval.evaluate();
	}
	else if(options_.output_option() == NMAPPINGS) {
		std::stringstream s;
		Evaluation eval(&B, &R, *document_stream_, s,
						options_.early_output(), options_.line_by_line());
		eval.evaluate();
		std::cout << eval.enumerator->numMappings() << "\n";
	}
	else {
		Evaluation eval(&B, &R, *document_stream_, std::cout,
						options_.early_output(), options_.line_by_line());
		eval.evaluate();
	}
}

void Interface::benchmarkRun() {
    std::stringstream output;

	size_t eVASize, detASize, numOfSpans,
		numOfNodes, numOfNodeArenas,
		numOfNodesReused;
	double initAutomataTime, evaluateTime, totTime, memoryUsage;
	/**************************** Run Algorithm ****************************/

	Timer t; 								// Start timer for automata creation

	if(options_.output_option() == DEBUG) {
		std::cout << "LVA:\n" << lv_automaton_.pprint() << "\n\n";
		std::cout << "auxLVA:\n" << aux_automaton_.pprint() << "\n\n";
	}

	ExtendedVA B = ExtendedVA(lv_automaton_);
	ExtendedVA R(aux_automaton_);

	if(options_.output_option() == DEBUG) {
		std::cout << "Filter fact:\n" <<  B.fFact->pprint() << "\n";
		std::cout << "Var fact:\n" <<  B.vFact->pprint() << "\n";
		std::cout << "eVA:\n" << B.pprint() << "\n\n";
		std::cout << "auxeVA:\n" << R.pprint() << "\n\n";
	}

	Evaluation eval(&B, &R, *document_stream_, output,
					options_.early_output(), options_.line_by_line());

	initAutomataTime = t.elapsed(); 		// Automata creation time
	t.reset(); 								// Start timer for evaluation time

	eval.evaluate();

	evaluateTime = t.elapsed(); 			// Evaluation time

	if(options_.output_option() == DEBUG) {
		std::cout << "Evaluation successful\n";
		std::cout << "\ndetA:\n" << eval.detA->pprint() << "\n\n";
		std::cout << "\nauxDetA:\n" << eval.auxDetA->pprint() << "\n\n";
	}

	/************************** Get Measurments **************************/

	totTime = initAutomataTime + evaluateTime;

	eVASize = B.size();
	detASize = eval.detA->size();

	numOfSpans = eval.enumerator->numMappings();

	numOfNodes = eval.memManager->totNodes();
	numOfNodeArenas = eval.memManager->totNodeArenas();
	numOfNodesReused = eval.memManager->totNodesReused();

	// GET MEMORY USAGE
	struct rusage usage;
	int ret;
	std::string memoryUsed;
	ret = getrusage(RUSAGE_SELF, &usage);
	if(ret == 0) {
		memoryUsed = formatMem(usage.ru_maxrss*1024);
	}

	/************************ Output Measurments ************************/

	std::cout 	<< "ExtendedVA size\t\t\t"			<< 	eVASize					<< 	'\n'
				<< "DetVA size\t\t\t" 				<< 	detASize				<<	'\n'
				<< "Number of mappings\t\t" 		<< 	numOfSpans				<<	'\n'
				<< "Number of nodes\t\t\t"			<<	numOfNodes 				<< 	'\n'
				<< "Number of node arenas\t\t"		<<	numOfNodeArenas 		<< 	'\n'
				<< "Memory used \t\t\t"				<<	memoryUsed	 			<< 	'\n'
				<< "Nodes reused\t\t\t"				<<	numOfNodesReused 		<< 	'\n'
				<< "Init Automata time\t\t"			<<	initAutomataTime 		<< 	"s\n"
				<< "Evaluate time\t\t\t"			<<	evaluateTime			<< 	"s\n"
				<< "Total time\t\t\t"				<<	totTime 				<< 	"s\n";

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
