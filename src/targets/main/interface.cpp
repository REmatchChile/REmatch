#include <sstream>
#include <istream>
#include <iostream>
#include <fstream>

#include <sys/resource.h>

#include "interface.hpp"
#include "parse/regex/parser.hpp"
#include "automata/nfa/eva.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "parse/automata/parser.hpp"
#include "util/timer.hpp"
#include "regex/regex.hpp"
#include "regex/regex_options.hpp"
#include "matchiterator.hpp"
#include "evaluation/normal_evaluator.hpp"

Interface::Interface(std::string &docstr, const std::string &pattern,
					 					 rematch::Options opt)
		:	document_(nullptr),
			pattern_(pattern),
			options_(opt) {
	if(opt.is_docfile()) {
		document_ = std::make_shared<rematch::FileDocument>(docstr);
	} else {
		document_ = std::make_shared<rematch::StrDocument>(docstr);
	}
}

void Interface::run() {
    if(options_.output_option() == rematch::BENCHMARK || options_.output_option() == rematch::DEBUG) {
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

	rematch::RegEx regex(pattern_, rgx_opts);

	rematch::MatchIterator m_iter = regex.findIter(document_);

	auto out_option = options_.output_option();

	if(out_option == rematch::SPANS) {
		for(auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
			std::cout << *match << std::endl;
		}
	} else if(out_option == rematch::NMAPPINGS) {
		size_t count = 0;
		for(auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
			count++;
		}
		std::cout << count << std::endl;
	} else if(out_option == rematch::SUBMATCHES) {
		auto str_doc = std::static_pointer_cast<rematch::StrDocument>(document_);
		for(auto match = m_iter.next(); match != nullptr; match = m_iter.next()) {
			std::cout << *match  << "\t(" << match->pprint(str_doc) << ")"  << std::endl;
		}
	}
}

void Interface::benchmark_run() {
    std::stringstream output;

	size_t n_mappings, numOfCaptures, numOfReadings, detSize, nfaSize, mdfaSize;
	double initAutomataTime, evaluateTime, totTime;
	/**************************** Run Algorithm ****************************/

	Timer t; 								// Start timer for automata creation

	rematch::RegExOptions rgx_opt;
	rgx_opt.set_line_by_line(options_.line_by_line());
	rgx_opt.set_early_output(options_.early_output());
	rematch::RegEx regex(pattern_, rgx_opt);
	rematch::Match_ptr match_ptr;

	initAutomataTime = t.elapsed(); 		// Automata creation time
	t.reset(); 													// Start timer for evaluation time

	n_mappings = 0;

	rematch::MatchIterator match_iter = regex.findIter(document_);

	for(auto match = match_iter.next(); match != nullptr; match = match_iter.next()) {
		n_mappings++;
	}

	numOfCaptures = regex.capture_counter();
	numOfReadings = regex.reading_counter();

	detSize = regex.dfa_counter();
	nfaSize = regex.nfa_counter();
	mdfaSize = regex.mdfa_counter();


	// std::cout << "\nRaw DFA:\n" <<  regex.detManager().DFA().pprint() << '\n';
	// std::cout << "\nRaw NFA:\n" <<  regex.detManager().NFA().pprint() << '\n';

	evaluateTime = t.elapsed(); 			// Evaluation time

	/************************** Get Measurments **************************/

	totTime = initAutomataTime + evaluateTime;

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
	<< "Number of mappings\t\t" 			<< 	pwc(n_mappings)											<<	'\n'
	<< "Memory used \t\t\t"						<<	memoryUsed	 												<< 	'\n'
	<< "MDFASize \t\t\t"							<<	mdfaSize														<<	'\n'
	<< "DetSize \t\t\t"								<<	detSize															<<	'\n'
	<< "eVASize \t\t\t"								<<	nfaSize															<< 	'\n'
	<< "Init Automata time\t\t"				<<	pwc(initAutomataTime) 							<< 	" ms\n"
	<< "Evaluate time\t\t\t"					<<	pwc(evaluateTime)										<< 	" ms\n"
	<< "Total time\t\t\t"							<<	pwc(totTime) 												<< 	" ms\n\n";

	#ifdef COUNT_CURRENT_STATES
	auto max_pair = match_iter.evaluator_->max_count_states();
	std::cout << "Max state count:\n" << max_pair.first
						<< "\t" << max_pair.second << "\n";
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

	while(sizeForm > 1024) {
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
			insertPosition-=3;
	}
	return numWithCommas;
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